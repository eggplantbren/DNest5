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

        // A database connection for I/O
        Database database;

        // A set of options
        const Options options;

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
    auto& db = database.db;
    if(options.clear_previous)
    {
        std::cout << "Clearing previous runs." << std::endl;
        db << "BEGIN;";
        database.clear_previous();
        db << "COMMIT;";
    }
//    std::cout << "Setting up sampler.
}

} // namespace

#endif

