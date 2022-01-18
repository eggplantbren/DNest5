#ifndef DNest5_Template_MyModel_hpp
#define DNest5_Template_MyModel_hpp

#include <cmath>
#include <UniformModel.hpp>
#include <Tools/Misc.hpp>

namespace DNest5_Template
{

using DNest5::ParameterNames, Tools::RNG;

class MyModel
{
    private:
        double x;

    public:
        inline MyModel(RNG& rng);
        inline double perturb(RNG& rng);
        inline double log_likelihood() const;
        inline std::vector<char> to_blob() const;
        inline void from_blob(const std::vector<char>& blob);
        inline std::string to_string() const;

        static ParameterNames parameter_names;
};

/* Implementations follow */

ParameterNames MyModel::parameter_names = std::vector<std::string>{"x"};

inline MyModel::MyModel(RNG& rng)
:x(rng.rand())
{

}

inline double MyModel::perturb(RNG& rng)
{
    x += rng.randh();
    Tools::wrap(x);

    return 0.0;
}

inline double MyModel::log_likelihood() const
{
    double logl = 0.0;
    return logl;
}

inline std::vector<char> MyModel::to_blob() const
{
    std::vector<char> result(1*sizeof(x));
    auto pos = &result[0];
    for(double value: {x})
    {
        std::memcpy(pos, &value, sizeof(value));
        pos += sizeof(value);
    }
    return result;
}

inline void MyModel::from_blob(const std::vector<char>& blob)
{
    auto pos = &blob[0];
    for(double* value: {&x})
    {
        std::memcpy(value, pos, sizeof(double));
        pos += sizeof(double);
    }
}


inline std::string MyModel::to_string() const
{
    return Tools::render(std::vector<double>{x}, ",");
}


} // namespace

#endif

