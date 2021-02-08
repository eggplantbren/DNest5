#ifndef DNest5_Levels_h
#define DNest5_Levels_h

#include "Particle.h"
#include "Options.h"

#include <algorithm>
#include <optional>
#include <Tools/Misc.hpp>
#include <vector>

namespace DNest5
{

// Using declarations
using Tools::minus_infinity;

/* Manage all levels in a pseudo-database here. */
class Levels
{
    private:

        // A copy of the options being used
        Options options;

        // Parallel arrays
        std::vector<double> logxs;
        std::vector<Pair> pairs;
        std::vector<double> log_push;
        bool push_is_active;

        // Statistics
        std::vector<unsigned long long> exceeds, visits, accepts, tries;

        // Stash of (logl_tb) pairs for new level creation
        std::vector<Pair> stash;

    public:

        // Initialise, passing options
        Levels(const Options& _options);

        // Add to stash. Return value = whether a new level was created.
        void add_to_stash(Pair&& pair);

        // Create a new level - IF the stash is large enough for it.
        // Return value is true if a level is actually created
        bool create_level();

        // Record stats
        template<typename T>
        inline void record_stats(const Particle<T>& particle, bool accepted);

        // Revise logxs
        void revise();

        // Adjust exceeds, visits, accepts, tries of the given level
        void adjust(int level, int e, int v, int a, int t);

        // Clear the stash
        void clear_stash();

        // Import stash points. Assumes levels are the same and merely
        // copies points over.
        void import_stash_from(const Levels& other);

        // Recent change in level log likelihood
        double recent_logl_changes() const;

        // Various getters
        inline int get_num_levels() const { return int(logxs.size()); }
        inline double get_logx(int level) const { return logxs[level]; }
        inline const Pair& get_pair(int level) const { return pairs[level]; }
        inline const Pair& get_top() const { return pairs.back(); }
        inline double get_log_push(int level) const { return log_push[level]; }
        inline unsigned long long get_exceeds(int level) const
        { return exceeds[level]; }
        inline unsigned long long get_visits(int level) const
        { return visits[level]; }
        inline unsigned long long get_accepts(int level) const
        { return accepts[level]; }
        inline unsigned long long get_tries(int level) const
        { return tries[level]; }
        inline bool get_push_is_active() const
        { return push_is_active; }
};

/* TEMPLATE IMPLEMENTATIONS */


// Record stats
template<typename T>
inline void Levels::record_stats(const Particle<T>& particle, bool accepted)
{
    const auto& [t, logl, tb, level] = particle;

    // Exceeds and visits
    for(int i=level; i<(int(logxs.size()) - 1); ++i)
    {
        ++visits[i];
        if(pairs[i+1] < Pair{logl, tb})
            ++exceeds[i];
        else
            break;
    }

    // Accepts and tries
    if(accepted)
        ++accepts[level];
    ++tries[level];
}


} // namespace


#endif
