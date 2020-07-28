#ifndef DNest5_SpikeSlab_hpp
#define DNest5_SpikeSlab_hpp

#include <cstring>
#include <DNest5/Misc.hpp>
#include <DNest5/RNG.hpp>
#include <vector>

namespace DNest5
{

class SpikeSlab
{
    private:

        // Parameters
        std::vector<double> xs;

    public:

        SpikeSlab(RNG& rng);
        double perturb(RNG& rng);
        double log_likelihood() const;
        std::vector<char> to_blob() const;
        void from_blob(const std::vector<char>& vec);
};

/* Implementations follow */

SpikeSlab::SpikeSlab(RNG& rng)
:xs(20)
{
    for(double& x: xs)
        x = rng.rand();
}

double SpikeSlab::perturb(RNG& rng)
{
    int num = 1;
    if(rng.rand() <= 0.5)
        num = int(pow(xs.size(), rng.rand()));

    for(int i=0; i<num; ++i)
    {
        int k = rng.rand_int(xs.size());
        xs[k] += rng.randh();
        wrap(xs[k], 0.0, 1.0);
    }

    return 0.0;
}


double SpikeSlab::log_likelihood() const
{
    double logL1 = 0.0;
    double logL2 = 0.0;

    static constexpr double u = 0.1;
    static constexpr double v = 0.01;
    static constexpr double shift = 0.031;
    static constexpr double C1 = -0.5*log(2*M_PI*u*u);
    static constexpr double C2 = -0.5*log(2*M_PI*v*v);
    static constexpr double tau1 = 1.0/(u*u);
    static constexpr double tau2 = 1.0/(v*v);
    static constexpr double log_hundred = log(100.0);

    for(double x: xs)
    {
        logL1 += C1 - 0.5*tau1*pow(x - 0.5, 2);
        logL2 += C2 - 0.5*tau2*pow(x - 0.5 - shift, 2);
    }
    return logsumexp(std::vector<double>{logL1, log_hundred + logL2});
}

std::vector<char> SpikeSlab::to_blob() const
{
    // Could just return xs, but writing this in full
    // to show how it would be done if the parameter space were
    // more complex
    int chars_per_element = sizeof(double)/sizeof(char);
    int chars_needed = xs.size()*chars_per_element;
    std::vector<char> result(chars_needed);
    int j = 0;
    for(size_t i=0; i<xs.size(); ++i)
    {
        std::memcpy(&result[j], &xs[i], sizeof(double));
        j += chars_per_element;
    }
    return result;
}

void SpikeSlab::from_blob(const std::vector<char>& vec)
{
    int chars_per_element = sizeof(double)/sizeof(char);
    int chars_needed = xs.size()*chars_per_element;
    assert(int(vec.size()) == chars_needed);

    int j = 0;
    for(size_t i=0; i<xs.size(); ++i)
    {
        std::memcpy(&xs[i], &vec[j], sizeof(double));
        j += chars_per_element;
    }
}

} // namespace

#endif

