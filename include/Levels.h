#ifndef DNest5_Levels_h
#define DNest5_Levels_h

#include <algorithm>
#include <DNest5/Particle.hpp>
#include <DNest5/Options.hpp>
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
        void record_stats(const Particle<T>& particle, bool accepted);

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
        int get_num_levels() const { return int(logxs.size()); }
        double get_logx(int level) const { return logxs[level]; }
        const Pair& get_pair(int level) const { return pairs[level]; }
        const Pair& get_top() const { return pairs.back(); }
        double get_log_push(int level) const { return log_push[level]; }
        unsigned long long get_exceeds(int level) const { return exceeds[level]; }
        unsigned long long get_visits(int level) const { return visits[level]; }
        unsigned long long get_accepts(int level) const { return accepts[level]; }
        unsigned long long get_tries(int level) const { return tries[level]; }
        bool get_push_is_active() const { return push_is_active; }
};

} // namespace


#endif
