#ifndef DNest5_Rosenbrock_hpp
#define DNest5_Rosenbrock_hpp

#include <cstring>
#include <DNest5/Misc.hpp>
#include <DNest5/RNG.hpp>
#include <vector>

namespace DNest5
{

class Rosenbrock
{
    private:

        // Parameters
        std::vector<double> xs;

    public:

        Rosenbrock(RNG& rng);
        double perturb(RNG& rng);
        double log_likelihood() const;
        std::vector<char> to_blob() const;
        void from_blob(const std::vector<char>& vec);
};

/* Implementations follow */

Rosenbrock::Rosenbrock(RNG& rng)
:xs(50)
{
    for(double& x: xs)
        x = -10.0 + 20.0*rng.rand();
}

double Rosenbrock::perturb(RNG& rng)
{
    int num = 1;
    if(rng.rand() <= 0.5)
        num = int(pow(xs.size(), rng.rand()));

    for(int i=0; i<num; ++i)
    {
        int k = rng.rand_int(xs.size());
        xs[k] += 20.0*rng.randh();
        wrap(xs[k], -10.0, 10.0);
    }

    return 0.0;
}


double Rosenbrock::log_likelihood() const
{
    double logl = 0.0;

    for(size_t i=0; i<(xs.size()-1); ++i)
    {
        logl -= 100*pow(xs[i+1] - xs[i]*xs[i], 2);
        logl -= pow(1.0 - xs[i], 2);
    }

    logl *= 2;

    return logl;
}

std::vector<char> Rosenbrock::to_blob() const
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

void Rosenbrock::from_blob(const std::vector<char>& vec)
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

