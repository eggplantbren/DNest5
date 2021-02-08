#include "Options.h"

namespace DNest5
{

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
                 std::optional<int> _rng_seed)
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
{
    std::cout << std::setprecision(stdout_precision);
    assert(save_interval % num_threads == 0);
    assert(num_particles % num_threads == 0);
    assert(max_num_saves % num_threads == 0);
}

Options::Options(const char* yaml_file)
{
    // Load the file
    YAML::Node file = YAML::LoadFile(yaml_file);

    // Get all the nodes
    num_particles = file["num_particles"].as<int>();
    num_threads = file["num_threads"].as<int>();
    new_level_interval = file["new_level_interval"].as<int>();
    save_interval = file["save_interval"].as<int>();
    thin = file["thin"].as<double>();
    try
    {
        max_num_levels = file["max_num_levels"].as<int>();
    }
    catch(const YAML::TypedBadConversion<int>& e)
    {
        max_num_levels = std::optional<int>();
    }
    lambda = file["lambda"].as<double>();
    beta = file["beta"].as<double>();
    max_num_saves = file["max_num_saves"].as<int>();
    try
    {
        rng_seed = file["rng_seed"].as<int>();
    }
    catch(const YAML::TypedBadConversion<int>& e)
    {
        // Use the time
        rng_seed = time(0);
    }

    assert(save_interval % num_threads == 0);
    assert(num_particles % num_threads == 0);
    assert(max_num_saves % num_threads == 0);
}

} // namespace

