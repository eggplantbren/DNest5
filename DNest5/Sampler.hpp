#ifndef DNest5_Sampler_hpp
#define DNest5_Sampler_hpp

#include <DNest5/Database.hpp>
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


    public:

        // Construct with a set of options.
        Sampler(Options _options = Options());

};

/* IMPLEMENTATIONS FOLLOW */

template<typename T>
Sampler<T>::Sampler(Options _options)
:options(_options)
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

    // Generate initial particles
    std::cout << "    Generating " << options.num_particles << " particles ";
    std::cout << "from the prior..." << std::flush;
    particles.reserve(options.num_particles);
    for(int i=0; i<options.num_particles; ++i)
    {
        T t(rng);
        double logl = t.log_likelihood();
        double tb = rng.rand();
        int level = 0;
        db << "INSERT INTO particles (sampler, params, logl, tb, level)\
                VALUES (?, ?, ?, ?, ?);"
           << sampler_id << t.to_blob() << logl << tb << level;
        particles.emplace_back(std::move(t), logl, tb, level);
    }
    std::cout << "done." << std::endl;

    db << "COMMIT;";
}

} // namespace

#endif

