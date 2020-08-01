#ifndef DNest5_Options_hpp
#define DNest5_Options_hpp

#include <cassert>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <optional>

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
        bool clear_previous;

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
                std::optional<int> _rng_seed = std::optional<int>(),
                bool _clear_previous = true);

        // Constructor that loads from a YAML file
        Options(const char* yaml_file);

        // Stuff the user doesn't need to control
        static constexpr auto db_filename = ".db/dnest5.db";
        static constexpr int stdout_precision = 12;
        static constexpr int rng_seed_gap = 123;
        static constexpr int level_save_gap = 10;

        // Friends
        template<typename T>
        friend class Sampler;

        friend class Levels;
};

/* Implementations follow */

Options::Options(int _num_particles,
                 int _num_threads,
                 int _new_level_interval,
                 int _save_interval,
                 double _thin,
                 std::optional<int> _max_num_levels,
                 double _lambda,
                 double _beta,
                 int _max_num_saves,
                 std::optional<int> _rng_seed,
                 bool _clear_previous)
:num_particles(_num_particles)
,num_threads(_num_threads)
,new_level_interval(_new_level_interval)
,save_interval(_save_interval)
,thin(_thin)
,max_num_levels(_max_num_levels)
,lambda(_lambda)
,beta(_beta)
,max_num_saves(_max_num_saves)
,rng_seed(_rng_seed.value_or(time(0)))
,clear_previous(_clear_previous)
{
    std::cout << std::setprecision(stdout_precision);
    assert(save_interval % num_threads == 0);
    assert(num_particles % num_threads == 0);
    assert(max_num_saves % num_threads == 0);
}

Options::Options(const char* yaml_file)
:Options()
{
    std::cerr << "Not opening " << yaml_file << ": YAML Options files ";
    std::cerr << "not implemented yet. Falling back to defaults." << std::endl;
}

} // namespace

#endif

