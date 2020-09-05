#ifndef DNest5_Rosenbrock_hpp
#define DNest5_Rosenbrock_hpp

#include <DNest5/UniformModel.hpp>

namespace DNest5
{

class Rosenbrock : public UniformModel<50, Rosenbrock>
{
    private:

    public:

        Rosenbrock(RNG& rng);
        void us_to_params();
        double log_likelihood() const;
};

/* Implementations follow */

Rosenbrock::Rosenbrock(RNG& rng)
:UniformModel(rng)
{
    us_to_params();
}

void Rosenbrock::us_to_params()
{
    for(size_t i=0; i<xs.size(); ++i)
        xs[i] = -10.0 + 20.0*us[i];
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

} // namespace

#endif

