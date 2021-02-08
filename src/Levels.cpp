#include "Levels.h"


namespace DNest5
{

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
        double numerator   = e + 100.0*exp(-1.0);
        double denominator = v + 100.0;
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
