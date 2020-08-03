#ifndef DNest5_SpikeSlab_hpp
#define DNest5_SpikeSlab_hpp

#include <DNest5/UniformModel.hpp>

namespace DNest5
{

class SpikeSlab:public UniformModel<20>
{
    private:


    public:

        SpikeSlab(RNG& rng);
        void us_to_params();
        double log_likelihood() const;
};

/* Implementations follow */

SpikeSlab::SpikeSlab(RNG& rng)
:UniformModel(rng)
{
    us_to_params();
}

void SpikeSlab::us_to_params()
{
    xs = us;
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


} // namespace

#endif

