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

/* IMPLEMENTATIONS FOLLOW */

Levels::Levels(const Options& _options)
:options(_options)
,logxs{0.0}
,pairs{{minus_infinity, 0.0}}
,log_push{0.0}
,push_is_active(true)
,exceeds{0}, visits{0}, accepts{0}, tries{0}
{
    // Reserve some RAM
    if(options.max_num_levels.has_value())
    {
        logxs.reserve(*options.max_num_levels);
        pairs.reserve(*options.max_num_levels);
        log_push.reserve(*options.max_num_levels);
        exceeds.reserve(*options.max_num_levels);
        visits.reserve(*options.max_num_levels);
        accepts.reserve(*options.max_num_levels);
        tries.reserve(*options.max_num_levels);
    }
    stash.reserve(int(1.5*options.new_level_interval));
}

void Levels::add_to_stash(Pair&& pair)
{
    // Bail if this is a cow's opinion
    if((options.max_num_levels.has_value()
            && int(logxs.size()) >= *options.max_num_levels)
        || !push_is_active)
    {
        if(stash.size() > 0)
            stash.clear();
        return;
    }

    if(pairs.back() < pair)
    {
        // Put the pair in the stash
        stash.emplace_back(pair);
    }
}

bool Levels::create_level()
{
    // Don't do anything if the stash is too small
    if(int(stash.size()) < options.new_level_interval)
        return false;

    // Sort the stash and find the quantile
    std::sort(stash.begin(), stash.end());
    int idx = int(0.6321206*stash.size());
    logxs.push_back(logxs.back() - 1.0);
    pairs.push_back(stash[idx]);
    exceeds.push_back(0);
    visits.push_back(0);
    accepts.push_back(0);
    tries.push_back(0);
    log_push.push_back(0.0);
    stash.clear();

    // See if push should be disabled
    if(options.max_num_levels.has_value())
    {
        if(int(logxs.size()) >= *options.max_num_levels)
            push_is_active = false;
    }
    else
    {
        if(recent_logl_changes() <= 0.5)
            push_is_active = false;
    }
    if(!push_is_active)
        std::cout << "Done creating levels." << std::endl;

    // Recompute log_push
    for(int i=0; i<int(logxs.size()); ++i)
    {
        if(push_is_active)
        {
            double dist = double(logxs.size()) - 1.0 - double(i);
            log_push[i] = -0.5*pow(dist/options.lambda, 2);
        }
        else
            log_push[i] = 0.0;
    }

    std::cout << "Created level " << logxs.size() << " with logl = ";
    std::cout << std::get<0>(pairs.back()) << "." << std::endl;

    return true;
}

void Levels::revise()
{
    for(int i=1; i<int(logxs.size()); ++i)
    {
        double e = exceeds[i-1];
        double v = visits[i-1];

        double numerator   = e + exp(-1.0)*options.new_level_interval;
        double denominator = v + options.new_level_interval;
        logxs[i] = logxs[i-1] + log(numerator/denominator);
    }
}

void Levels::clear_stash()
{
    stash.clear();
}

void Levels::import_stash_from(const Levels& other)
{
    for(const auto& point: other.stash)
        stash.push_back(point);
}

void Levels::adjust(int level, int e, int v, int a, int t)
{
    if(level >= int(logxs.size()))
        return;
    exceeds[level] += e;
    visits[level] += v;
    accepts[level] += a;
    tries[level] += t;
}


// Record stats
template<typename T>
void Levels::record_stats(const Particle<T>& particle, bool accepted)
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


double Levels::recent_logl_changes() const
{
    int end   = int(logxs.size());
    int start = end - 20;
    if(start < 1)
        start = 1;
    double numerator   = 0.0;
    double denominator = 0.0;
    for(int i=start; i<end; ++i)
    {
        numerator   += (i-start+1)*(std::get<0>(pairs[i]) - std::get<0>(pairs[i-1]));
        denominator += (i-start+1);
    }
    return numerator/denominator;
}


} // namespace


#endif
