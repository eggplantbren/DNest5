#ifndef DNest5_Levels_hpp
#define DNest5_Levels_hpp

#include <DNest5/Misc.hpp>
#include <DNest5/Particle.hpp>
#include <optional>
#include <vector>

namespace DNest5
{

/* Manage all levels in a pseudo-database here. */
class Levels
{
    private:

        // A copy of some options
        int new_level_interval;
        std::optional<int> max_num_levels;        

        // Parallel arrays
        std::vector<Pair> logl_tb_pairs;

        // Stash of (logl_tb) pairs for new level creation
        std::vector<Pair> stash;

    public:
        // No default constructor
        Levels() = delete;

        // Initialise empty, but notified of some options
        Levels(int new_level_interval, std::optional<int> _max_num_levels);

        // Get the level thresholds
        const std::vector<Pair>& get_logl_tb_pairs() const
        { return logl_tb_pairs; }

};

/* IMPLEMENTATIONS FOLLOW */

Levels::Levels(int _new_level_interval, std::optional<int> _max_num_levels)
:new_level_interval(_new_level_interval)
,max_num_levels(_max_num_levels)
,logl_tb_pairs{{minus_infinity, 0.0}}
{
    // Reserve some RAM
    if(max_num_levels.has_value())
        logl_tb_pairs.reserve(*max_num_levels);
    stash.reserve(int(1.5*new_level_interval));
}


} // namespace


#endif
