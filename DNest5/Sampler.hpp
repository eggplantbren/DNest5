#ifndef DNest5_Sampler_hpp
#define DNest5_Sampler_hpp

#include <DNest5/Database.hpp>
#include <DNest5/Levels.hpp>
#include <DNest5/Options.hpp>
#include <DNest5/Particle.hpp>
#include <DNest5/RNG.hpp>
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

        // The random number generator
        RNG rng;

        // The particles
        std::vector<Particle<T>> particles;

        // The levels
        Levels levels;

        // Count work
        unsigned long long work;

        // Do a Metropolis step of particle k, or of its level
        bool metropolis_step(int k);
        void metropolis_step_level(int k);

        // Save levels or particles
        void save_levels();
        void save_particle(int k, bool with_params);

        // Explore until a new level or save occurs.
        void explore();

    public:

        // Construct with a set of options.
        Sampler(Options _options = Options());

        // Run until termination
        void run();
};

/* IMPLEMENTATIONS FOLLOW */

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

    // Make sure the RNG seed is unused. Choose a new one if necessary.
    int count;
    do
    {
        db << "SELECT COUNT(*) FROM samplers WHERE rng_seed = ?;"
           << options.rng_seed >> count;
        if(count != 0)
            options.rng_seed -= Options::rng_seed_gap;
    }while(count != 0);
    std::cout << "    RNG seed = " << options.rng_seed << "." << std::endl;

    // Save sampler info to the database
    db << "INSERT INTO samplers\
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);"
       << sampler_id << options.new_level_interval << options.save_interval
       << options.metadata_save_interval << options.max_num_levels
       << options.lambda << options.beta << options.max_num_saves
       << options.rng_seed;

    // Save level info to the database
    save_levels();

    // Generate initial particles
    std::cout << "    Generating " << options.num_particles << " particles ";
    std::cout << "from the prior..." << std::flush;
    particles.reserve(options.num_particles);
    for(int i=0; i<options.num_particles; ++i)
    {
        int level = 0;
        T t(rng);
        double logl = t.log_likelihood();
        double tb = rng.rand();
        particles.emplace_back(std::move(t), logl, tb, level);
        save_particle(i, true);
    }
    std::cout << "done." << std::endl;

    db << "COMMIT;";
}

template<typename T>
void Sampler<T>::run()
{
    while(database.num_full_particles(sampler_id) < options.max_num_saves)
        explore();
}

template<typename T>
void Sampler<T>::explore()
{
    std::cout << "Exploring." << std::endl;

    // Local handy alias
    auto& db = database.db;
    db << "BEGIN;";
    while(true)
    {
        // Choose a particle
        int k = rng.rand_int(options.num_particles);

        // Do a Metropolis step
        metropolis_step(k);

        // Add to stash
        bool level_created = levels.add_to_stash(logl_tb(particles[k]));
        if(level_created)
            save_levels();

        // Increment work
        ++work;

        // Save a particle
        bool full_save = work % options.save_interval == 0;
        if(work % options.metadata_save_interval == 0)
            save_particle(k, full_save);

        // Break out of the loop
        if(full_save || level_created)
            break;
    }
    db << "COMMIT;";

    std::cout << "Work done = ";
    std::cout << std::scientific << std::setprecision(3);
    std::cout << double(work) << "." << std::endl;
    std::cout << std::defaultfloat;
    std::cout << std::setprecision(options.stdout_precision);
    std::cout << std::endl;
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
                 visits, exceeds, tries, accepts)\
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);"
           << sampler_id << i << levels.get_logx(i) << logl << tb
           << levels.get_visits(i) << levels.get_exceeds(i)
           << levels.get_tries(i)  << levels.get_accepts(i);
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

    if(!level_first)
        metropolis_step_level(k);

    return accepted;
}


template<typename T>
void Sampler<T>::metropolis_step_level(int k)
{
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

    if(level_prop < level)
        loga += levels.get_logx(level) - levels.get_logx(level_prop);

    // Accept
    if(rng.rand() <= exp(loga))
        level = level_prop;
}

} // namespace

#endif

