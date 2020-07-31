#ifndef DNest5_Sampler_hpp
#define DNest5_Sampler_hpp

#include <DNest5/Database.hpp>
#include <DNest5/Levels.hpp>
#include <DNest5/Options.hpp>
#include <DNest5/Particle.hpp>
#include <DNest5/RNG.hpp>
#include <mutex>
#include <thread>
#include <vector>

namespace DNest5
{

/* The sampler class */
template<typename T>
class Sampler
{
    private:

        // Unique sampler ID
        int sampler_id;

        // A database connection for I/O
        Database database;

        // A set of options
        Options options;

        // The random number generators
        std::vector<RNG> rngs;

        // The particles
        std::vector<Particle<T>> particles;

        // The levels
        Levels levels;

        // Count work
        unsigned long long work;

        // Mutexes
        static std::mutex levels_mutex, database_mutex;

        // Do a Metropolis step of particle k, or of its level
        bool metropolis_step(int k);
        void metropolis_step_level(int k);

        // Save levels or particles
        void save_levels();
        void save_particle(int k, bool with_params);

        // Explore until a new level or save occurs.
        void explore(int thread);

    public:

        // Construct with a set of options.
        Sampler(Options _options = Options());

        // Run until termination
        void run();
};

/* IMPLEMENTATIONS FOLLOW */

template<typename T>
std::mutex Sampler<T>::levels_mutex;

template<typename T>
std::mutex Sampler<T>::database_mutex;


template<typename T>
Sampler<T>::Sampler(Options _options)
:options(std::move(_options))
,levels(options)
,work(0)
{
    // Shorthand to database connection
    auto& db = database.db;
    db << "BEGIN;";

    // Clear previous data (if requested)
    if(options.clear_previous)
    {
        std::cout << "Clearing previous runs." << std::endl;
        database.clear_previous();
    }

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
       << options.metadata_save_interval << options.max_num_levels
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
    for(int i=0; i<options.num_particles; ++i)
    {
        int level = 0;
        T t(rng);
        double logl = t.log_likelihood();
        double tb = rng.rand();
        particles.emplace_back(std::move(t), logl, tb, level);
    }
    std::cout << "done." << std::endl;

    db << "COMMIT;";
}

template<typename T>
void Sampler<T>::run()
{
    while(database.num_full_particles(sampler_id) < options.max_num_saves)
    {
        std::cout << "Exploring." << std::endl;

        auto& db = database.db;
        db << "BEGIN;";
        std::vector<std::thread> threads;
        for(int thread=0; thread<options.num_threads; ++thread)
        {
            auto func = std::bind(&Sampler<T>::explore, this, thread);
            threads.emplace_back(func);
        }
        for(auto& thread: threads)
            thread.join();

        // Save one full particle
        save_particle(rngs[0].rand_int(options.num_particles), true);

        // Level work
        levels.revise();
        save_levels();
        db << "COMMIT;";

        work += options.save_interval;
        std::cout << "Work done = ";
        std::cout << std::scientific << std::setprecision(3);
        std::cout << double(work) << "." << std::endl;
        std::cout << std::defaultfloat;
        std::cout << std::setprecision(options.stdout_precision);
        std::cout << std::endl;
    }
}

template<typename T>
void Sampler<T>::explore(int thread)
{
    // Temporary
    auto& rng = rngs[thread];

    int k;
    int steps = options.save_interval/options.num_threads;
    for(int i=0; i<steps; ++i)
    {
        // Choose a particle
        k = thread*(options.num_particles/options.num_threads)
               + rng.rand_int(options.num_particles/options.num_threads);

        // Do a Metropolis step
        metropolis_step(k);

        // Add to stash
        levels_mutex.lock();
        bool level_created = levels.add_to_stash(logl_tb(particles[k]));
        levels_mutex.unlock();

        // Save a particle (metadata only)
        if((i+1) % options.metadata_save_interval == 0 &&
            i != (steps-1))
        {
            database_mutex.lock();
            save_particle(k, false);
            database_mutex.unlock();
        }
    }
}

template<typename T>
void Sampler<T>::save_levels()
{
    // Alias
    auto& db = database.db;

    // Rewrite all levels from scratch (for this sampler ID)
    db << "DELETE FROM levels WHERE sampler = ?;" << sampler_id;

    int num_levels = levels.get_num_levels();
    for(int i=0; i<num_levels; ++i)
    {
        const auto& [logl, tb] = levels.get_pair(i);
        db << "INSERT INTO levels\
                (sampler, level, logx, logl, tb,\
                 exceeds, visits, accepts, tries)\
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);"
           << sampler_id << i << levels.get_logx(i) << logl << tb
           << levels.get_exceeds(i) << levels.get_visits(i)
           << levels.get_accepts(i) << levels.get_tries(i);
    }
}

template<typename T>
void Sampler<T>::save_particle(int k, bool with_params)
{
    // Alias
    auto& db = database.db;

    // Unpack
    const auto& [t, logl, tb, level] = particles[k];

    // Maybe a blob
    std::optional<std::vector<char>> blob;
    if(with_params)
        blob = t.to_blob();

    // Insert into the DB
    db << "INSERT INTO particles (sampler, level, params, logl, tb)\
            VALUES (?, ?, ?, ?, ?);"
       << sampler_id << level << blob << logl << tb;

    // Stdout message
    if(with_params)
    {
        std::cout << "Saved particle ";
        std::cout << database.num_full_particles(sampler_id);
        std::cout << "." << std::endl;
    }

}



template<typename T>
bool Sampler<T>::metropolis_step(int k)
{
    // Return value
    bool accepted = false;

    // Temporary
    int thread = k/options.num_threads;
    auto& rng = rngs[thread];

    bool level_first = rng.rand() <= 0.5;
    if(level_first)
        metropolis_step_level(k);

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
        if(levels.get_pair(level) < Pair{logl_prop, tb_prop})
        {
            accepted = true;
            particle = proposal;
        }
    }

    // Record stats
    levels_mutex.lock();
    levels.record_stats(particle, accepted);
    levels_mutex.unlock();

    if(!level_first)
        metropolis_step_level(k);

    return accepted;
}


template<typename T>
void Sampler<T>::metropolis_step_level(int k)
{
    // Temporary
    auto& rng = rngs[0];

    // Unpack the particle and create a copy for the proposal
    auto& particle = particles[k];
    auto& [t, logl, tb, level] = particle;

    int mag  = 1 + int(std::abs(rng.randc()));
    int sign = (rng.rand() <= 0.5)?(-1):(1);
    int level_prop = level + mag*sign;
    if(level_prop < 0
            || level_prop >= levels.get_num_levels()
            || Pair{logl, tb} < levels.get_pair(level_prop))
        return;

    // Acceptance probability
    double loga = levels.get_log_push(level_prop) - levels.get_log_push(level);

    // Logx part for downward moves
    if(level_prop < level)
        loga += levels.get_logx(level) - levels.get_logx(level_prop);

    // Beta part
    if(!levels.get_push_is_active())
    {
        loga += options.beta*(log(1 + levels.get_tries(level))
                                    - log(1 + levels.get_tries(level_prop)));
    }

    // Accept
    if(rng.rand() <= exp(loga))
        level = level_prop;
}

} // namespace

#endif

