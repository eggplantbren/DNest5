#ifndef DNest5_Options_h
#define DNest5_Options_h

#include <cassert>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <optional>
#include <yaml-cpp/yaml.h>

namespace DNest5
{

// An Options object contains the parameters of a DNest5 run.
// The constructor allows sensible defaults, and eventually you could
// also load the options from a .yaml file.
class Options
{
    private:

        int num_particles;
        int num_threads;
        int new_level_interval;
        int save_interval;
        double thin;
        std::optional<int> max_num_levels;
        double lambda;
        double beta;
        int max_num_saves;
        int rng_seed;

    public:

        // Constructor that specifies everything (or, use the defaults)
        Options(int _num_particles = 5,
                int _num_threads = 5,
                int _new_level_interval = 10000,
                int _save_interval = 1000,
                double _thin = 0.1,
                std::optional<int> max_num_levels = 100, //std::optional<int>(),
                double _lambda = 10.0,
                double _beta = 100.0,
                int _max_num_saves = 100000,
                std::optional<int> _rng_seed = std::optional<int>());

        // Constructor that loads from a YAML file
        Options(const char* yaml_file);

        // Stuff the user doesn't need to control
        static constexpr auto db_filename = "output/dnest5.db";
        static constexpr int stdout_precision = 12;
        static constexpr int rng_seed_gap = 123;
        static constexpr int level_save_gap = 10;

        // Friends
        template<typename T>
        friend class Sampler;

        friend class Levels;
};


} // namespace

#endif

