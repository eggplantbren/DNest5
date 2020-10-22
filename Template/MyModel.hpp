#ifndef DNest5_Template_MyModel_hpp
#define DNest5_Template_MyModel_hpp

#include <cmath>
#include <DNest5/UniformModel.hpp>
#include <Tools/Misc.hpp>

namespace DNest5_Template
{

using DNest5::ParameterNames, Tools::RNG;

// The "Curiously Recursive" template argument is so that the correct
// naming scheme is used.
class MyModel : public DNest5::UniformModel<5, MyModel>
{
    private:

    public:
        MyModel(RNG& rng);
        void us_to_params();
        double log_likelihood() const;
};

/* Implementations follow */

MyModel::MyModel(RNG& rng)
:UniformModel(rng)
{
    us_to_params();
}

void MyModel::us_to_params()
{
    xs = us;
}

double MyModel::log_likelihood() const
{
    double logl = 0.0;
    return logl;
}

} // namespace

#endif

