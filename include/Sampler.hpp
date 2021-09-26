#ifndef DNest5_Sampler_hpp
#define DNest5_Sampler_hpp

#include "Database.h"
#include "Levels.h"
#include "Options.h"
#include "Particle.h"
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <Tools/Barrier.hpp>
#include <Tools/RNG.hpp>
#include <vector>

namespace DNest5
{

// Using declarations
using Tools::Barrier, Tools::RNG, Tools::wrap;

/* The sampler class */
template<typename T>
class Sampler
{
    private:

        // Unique sampler ID
        int sampler_id;

        // A database connection for I/O
        Database database;

        // Prepared statement to save particles
        std::optional<sqlite::database_binder> save_particle_ps;

        // A set of options
        Options options;

        // The random number generators
        std::vector<RNG> rngs;

        // The particles
        std::vector<Particle<T>> particles;

        // The levels
        Levels levels;

        // Copies of levels, for multithreading
        std::vector<Levels> levels_copies;

        // Count work
        unsigned long long work;
        unsigned long long saved_particles, saved_full_particles;
        bool done;

        // A barrier
        std::unique_ptr<Barrier> barrier;

        // Do a Metropolis step of particle k, or of its level
        inline bool metropolis_step(int k, int thread);
        inline void metropolis_step_level(int k, int thread);

        // Save levels or particles
        inline void save_levels();
        inline void save_particle(int k, bool with_params);

        // Explore until a new level or save occurs.
        inline void explore(int thread);

        // Run a thread
        inline void run_thread(int thread);

        // Prune lagging particles
        inline void prune_laggards();
        int pruned;

    public:

        // Construct with a set of options.
        inline Sampler(Options _options = Options());
        inline void run();
};

/* IMPLEMENTATIONS FOLLOW */

template<typename T>
inline Sampler<T>::Sampler(Options _options)
:options(std::move(_options))
,levels(options)
,levels_copies(options.num_threads, options)
,work(0)
,saved_particles(0)
,saved_full_particles(0)
,done(false)
,pruned(0)
{
    // Shorthand to database connection
    auto& db = database.db;
    db << "BEGIN;";

    // Initialise the sampler, first by setting a sampler ID.
    std::cout << "Initialising sampler:" << std::endl;
    sampler_id = 1;
    db << "SELECT MAX(id) FROM samplers;" >>
        [&](int max_id)
        {
            sampler_id = max_id + 1;
        };
    std::cout << "    Sampler ID = " << sampler_id << "." << std::endl;

    // Save sampler info to the database
    db << "INSERT INTO samplers\
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
       << sampler_id << options.num_particles << options.num_threads
       << options.new_level_interval << options.save_interval
       << options.thin << options.max_num_levels
       << options.lambda << options.beta << options.max_num_saves;

    // Starting with the hint given in Options, generate all the actual RNG
    // seeds.
    rngs.reserve(options.num_threads);
    int count;
    int seed = options.rng_seed;
    std::cout << "    RNG seeds = (";
    do
    {
        db << "SELECT COUNT(*) FROM rngs WHERE seed = ?;"
           << seed >> count;
        if(count == 0)
        {
            rngs.emplace_back(RNG());
            rngs.back().set_seed(seed);
            db << "INSERT INTO rngs VALUES (?, ?);" << seed << sampler_id;
            std::cout << seed;
            if(int(rngs.size()) < options.num_threads-1)
                std::cout << ", ";
            else
                std::cout << ")." << std::endl;
            seed -= Options::rng_seed_gap;
        }
    }while(int(rngs.size()) < options.num_threads);

    // Save level info to the database
    save_levels();

    // Generate initial particles
    std::cout << "    Generating " << options.num_particles << " particles ";
    std::cout << "from the prior..." << std::flush;
    particles.reserve(options.num_particles);
    auto& rng = rngs[0];
    // Always generate from the prior serially - so classes can assume that
    // e.g., static data can be loaded in a constructor
    for(int i=0; i<options.num_particles; ++i)
    {
        int level = 0;
        T t(rng);
        double logl = t.log_likelihood();
        double tb = rng.rand();
        particles.emplace_back(std::move(t), logl, tb, level);
    }
    std::cout << "done.\n" << std::endl;

    db << "COMMIT;";

    // Have to emplace here because operator = doesn't exist for database_binder type
    save_particle_ps.emplace(database.db << "INSERT INTO particles (sampler, level, params, logl, tb)\
               VALUES (?, ?, ?, ?, ?);");
}

template<typename T>
inline void Sampler<T>::run()
{
    // Create the barrier
    barrier.reset(new Barrier(options.num_threads));

    // Create the threads
    std::vector<std::thread> threads;
    for(int thread=0; thread<options.num_threads; ++thread)
    {
        auto func = std::bind(&Sampler<T>::run_thread, this, thread);
        threads.emplace_back(func);
    }

    // Join the threads
    for(auto& thread: threads)
        thread.join();

    // Save levels one last time
    database.db << "BEGIN;";
    save_levels();
    database.db << "COMMIT;";
}

template<typename T>
inline void Sampler<T>::run_thread(int thread)
{
    auto& db = database.db;

    while(true)
    {
        // Print a message and start DB transaction
        if(thread == 0 && !done)
        {
            std::cout << "Exploring ["
                      << levels.get_num_levels() << " levels, ";
            std::cout << "highest logl = "
                      << std::get<0>(levels.get_top()) << "]..." << std::flush;

            // Copy levels
            for(int i=0; i<options.num_threads; ++i)
            {
                levels_copies[i] = levels;
                levels_copies[i].clear_stash();
            }

            db << "BEGIN;";
        }

        // Do a bit of MCMC (or quit)
        barrier->wait();
        if(done)
            break;
        explore(thread);
        barrier->wait();

        if(thread == 0)
        {
            std::cout << "done." << std::endl;

            // Add to work done
            work += options.save_interval;

            // Save one particle
            int k = rngs[0].rand_int(options.num_particles);
            bool full = rngs[0].rand() <= options.thin;
            ++saved_particles;
            if(full)
                ++saved_full_particles;
            save_particle(k, full);
            done = saved_particles >= (unsigned int)options.max_num_saves;

            // Merge level data
            auto backup = levels;
            for(int i=0; i<options.num_threads; ++i)
            {
                for(int j=0; j<levels_copies[i].get_num_levels(); ++j)
                {
                    levels.adjust(j,
                     levels_copies[i].get_exceeds(j) - backup.get_exceeds(j),
                     levels_copies[i].get_visits(j) - backup.get_visits(j),
                     levels_copies[i].get_accepts(j) - backup.get_accepts(j),
                     levels_copies[i].get_tries(j) - backup.get_tries(j));
                }
                levels.import_stash_from(levels_copies[i]);
            }
            bool created_level = levels.create_level();

            // Level work
            levels.revise();
            if(created_level || (saved_full_particles % options.level_save_gap == 0))
                save_levels();
            db << "COMMIT;";

            // Check for any lagging particles
            prune_laggards();

            std::cout << "Work done = ";
            std::cout << std::scientific << std::setprecision(3);
            std::cout << double(work) << "." << std::endl;
            std::cout << std::defaultfloat;
            std::cout << std::setprecision(options.stdout_precision);
            std::cout << std::endl;
        }
    }
}

template<typename T>
inline void Sampler<T>::explore(int thread)
{
    // Temporary
    auto& rng = rngs[thread];

    int k;
    int steps = options.save_interval/options.num_threads;
    int particles_per_thread = options.num_particles/options.num_threads;
    for(int i=0; i<steps; ++i)
    {
        // Choose a particle
        k = thread*particles_per_thread + rng.rand_int(particles_per_thread);

        // Do a Metropolis step
        metropolis_step(k, thread);

        // Add to stash
        levels_copies[thread].add_to_stash(logl_tb(particles[k]));
    }
}

template<typename T>
inline void Sampler<T>::save_levels()
{
    // Alias
    auto& db = database.db;

    int num_levels = levels.get_num_levels();
    for(int i=0; i<num_levels; ++i)
    {
        const auto& [logl, tb] = levels.get_pair(i);

        unsigned long long e, v, a, t;
        e = levels.get_exceeds(i); v = levels.get_visits(i);
        a = levels.get_accepts(i); t = levels.get_tries(i);

        // Upsert each level
        db << "INSERT INTO levels\
               (id, logx, logl, tb, exceeds, visits, accepts, tries)\
               VALUES (?, ?, ?, ?, ?, ?, ?, ?)\
               ON CONFLICT (id) DO UPDATE\
               SET (logx, exceeds, visits, accepts, tries) = \
               (excluded.logx, excluded.exceeds, excluded.visits, \
                excluded.accepts, excluded.tries);"
           << i << levels.get_logx(i) << logl << tb
           << e << v << a << t;
    }
}

template<typename T>
inline void Sampler<T>::save_particle(int k, bool with_params)
{
    // Alias
    auto& db = database.db;

    // Unpack
    const auto& [t, logl, tb, level] = particles[k];

    // Maybe a blob
    std::optional<std::vector<char>> blob;
    if(with_params)
        blob = t.to_blob();

    // Bind values and execute prepared statement
    (*save_particle_ps) << sampler_id << level << blob << logl << tb;
    (*save_particle_ps)++;

    // Stdout message
    std::cout << "Saved particle ";
    std::cout << saved_particles << " [" << saved_full_particles << " ";
    std::cout << "full particles]." << std::endl;
}



template<typename T>
inline bool Sampler<T>::metropolis_step(int k, int thread)
{
    // Return value
    bool accepted = false;

    // Access correct RNG
    auto& rng = rngs[thread];

    bool level_first = rng.rand() <= 0.5;
    if(level_first)
        metropolis_step_level(k, thread);

    // Unpack the particle and create a copy for the proposal
    auto& particle = particles[k];
    auto proposal = particle;
    auto& [t, logl, tb, level] = particle;
    auto& [t_prop, logl_prop, tb_prop, level_prop] = proposal;

    // Make the proposal
    double logh = t_prop.perturb(rng);

    // Pre-reject
    if(rng.rand() <= exp(logh))
    {
        logl_prop = t_prop.log_likelihood();
        tb_prop += rng.randh(); wrap(tb_prop);
        if(levels_copies[thread].get_pair(level) < Pair{logl_prop, tb_prop})
        {
            accepted = true;
            particle = proposal;
        }
    }

    // Record stats
    levels_copies[thread].record_stats(particle, accepted);

    if(!level_first)
        metropolis_step_level(k, thread);

    return accepted;
}


template<typename T>
inline void Sampler<T>::metropolis_step_level(int k, int thread)
{
    // Access correct RNG
    auto& rng = rngs[thread];

    // Unpack the particle and create a copy for the proposal
    auto& particle = particles[k];
    auto& [t, logl, tb, level] = particle;

    int mag  = 1 + int(std::abs(rng.randc()));
    int sign = (rng.rand() <= 0.5)?(-1):(1);
    int level_prop = level + mag*sign;
    if(level_prop < 0
            || level_prop >= levels_copies[thread].get_num_levels()
            || Pair{logl, tb} < levels_copies[thread].get_pair(level_prop))
        return;

    // Acceptance probability
    double loga = levels_copies[thread].get_log_push(level_prop)
                        - levels_copies[thread].get_log_push(level);

    // Logx part for downward moves
    if(level_prop < level)
    {
        loga += levels_copies[thread].get_logx(level)
                    - levels_copies[thread].get_logx(level_prop);
    }

    // Beta part
    if(!levels.get_push_is_active())
    {
        loga += options.beta*(log(100 + levels_copies[thread].get_tries(level))
                    - log(100 + levels_copies[thread].get_tries(level_prop)));
    }

    // Accept
    if(rng.rand() <= exp(loga))
        level = level_prop;
}

template<typename T>
inline void Sampler<T>::prune_laggards()
{
    // Find log push of each particle
    std::vector<double> log_push(options.num_particles);
    for(int i=0; i<options.num_particles; ++i)
        log_push[i] = levels.get_log_push(std::get<3>(particles[i]));

    // Copy particles
    auto particles_copy = particles;
    for(int i=0; i<options.num_particles; ++i)
    {
        if(log_push[i] < -10.0)
        {
            int j = rngs[0].rand_int(options.num_particles);
            particles[i] = particles_copy[j];
            ++pruned;
        }
    }
    if(pruned > 0)
    {
        std::cout << pruned << " lagging particle";
        if(pruned == 1)
            std::cout << " has ";
        else
            std::cout << "s have ";
        std::cout << "been pruned." << std::endl;
    }
}

} // namespace

#endif

