#ifndef DNest5_Levels_hpp
#define DNest5_Levels_hpp

#include <algorithm>
#include <DNest5/Misc.hpp>
#include <DNest5/Particle.hpp>
#include <DNest5/Options.hpp>
#include <optional>
#include <vector>

namespace DNest5
{

/* Manage all levels in a pseudo-database here. */
class Levels
{
    private:

        // A copy of the options being used
        const Options& options;

        // Parallel arrays
        std::vector<double> logxs;
        std::vector<Pair> pairs;
        std::vector<double> log_push;

        // Statistics
        std::vector<unsigned long long> visits, exceeds;
        std::vector<unsigned long long> tries, accepts;

        // Stash of (logl_tb) pairs for new level creation
        std::vector<Pair> stash;

        // Create a new level
        void create_level();

    public:

        // Initialise, passing options
        Levels(const Options& _options);

        // Add to stash. Return value = whether a new level was created.
        bool add_to_stash(Pair&& pair);

        // Various getters
        int get_num_levels() const { return int(logxs.size()); }
        double get_logx(int level) const { return logxs[level]; }
        const Pair& get_pair(int level) const { return pairs[level]; }
        double get_log_push(int level) const { return log_push[level]; }
        unsigned long long get_visits(int level) const { return visits[level]; }
        unsigned long long get_exceeds(int level) const { return exceeds[level]; }
        unsigned long long get_tries(int level) const { return tries[level]; }
        unsigned long long get_accepts(int level) const { return accepts[level]; }
};

/* IMPLEMENTATIONS FOLLOW */

Levels::Levels(const Options& _options)
:options(_options)
,logxs{0.0}
,pairs{{minus_infinity, 0.0}}
,log_push{0.0}
,visits{0}, exceeds{0}
,tries{0},  accepts{0}
{
    // Reserve some RAM
    if(options.max_num_levels.has_value())
    {
        logxs.reserve(*options.max_num_levels);
        pairs.reserve(*options.max_num_levels);
        log_push.reserve(*options.max_num_levels);
        visits.reserve(*options.max_num_levels);
        exceeds.reserve(*options.max_num_levels);
        tries.reserve(*options.max_num_levels);
        accepts.reserve(*options.max_num_levels);
    }
    stash.reserve(int(1.5*options.new_level_interval));
}

bool Levels::add_to_stash(Pair&& pair)
{
    // Bail if this is a cow's opinion
    if(options.max_num_levels.has_value()
        && int(logxs.size()) >= *options.max_num_levels)
    {
        if(stash.size() > 0)
            stash.clear();
        return false;
    }

    if(pairs.back() < pair)
    {
        // Put the pair in the stash
        stash.emplace_back(pair);
    }

    if(int(stash.size()) >= options.new_level_interval)
    {
        create_level();
        return true;
    }

    return false;
}

void Levels::create_level()
{
    // Sort the stash and find the quantile
    std::sort(stash.begin(), stash.end());
    int idx = int(0.6321206*stash.size());
    logxs.push_back(logxs.back() - 1.0);
    pairs.push_back(stash[idx]);
    visits.push_back(0);
    exceeds.push_back(0);
    tries.push_back(0);
    accepts.push_back(0);
    log_push.push_back(0.0);
    stash.clear();

    // Recompute log_push
    bool push = !options.max_num_levels.has_value()
                      || int(logxs.size()) < *options.max_num_levels;
    for(int i=0; i<int(logxs.size()); ++i)
    {
        if(push)
        {
            double dist = double(logxs.size()) - 1.0 - double(i);
            log_push[i] = -log(1.0 + pow(dist/options.lambda, 2));
        }
        else
        log_push[i] = 0.0;
    }

    /* Eventually recompute logx values based on observed visits */
    std::cout << "Created level " << logxs.size() << " with logl = ";
    std::cout << std::get<0>(pairs.back()) << "." << std::endl;
}

} // namespace


#endif
