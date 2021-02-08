#ifndef DNest5_UniformModel_hpp
#define DNest5_UniformModel_hpp

#include "Options.h"
#include "ParameterNames.h"

#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <Tools/Misc.hpp>
#include <Tools/RNG.hpp>

namespace DNest5
{

// Using declarations
using Tools::logsumexp, Tools::RNG, Tools::qnorm, Tools::wrap;

/*
    Derive from this class to implement models using an underlying
    set of coordinates with Uniform(0, 1) priors.
*/
template<int num_params, typename T>
class UniformModel
{
    protected:

        // Underlying coordinates and their transformed version
        std::vector<double> us;
        std::vector<double> xs;

    public:

        // Default constructor sets up the vectors
        inline UniformModel(RNG& rng);
        inline virtual ~UniformModel() = default;

        // Functions specified here and not to be overridden
        inline double perturb(RNG& rng);
        inline std::vector<char> to_blob() const;
        inline void from_blob(const std::vector<char>& vec);
        inline std::string to_string() const;

        // Access parameters by name
        inline virtual double& param(std::string&& name);
        inline virtual const double& param(std::string&& name) const;

        // Functions to be specified in the derived class
        inline virtual void us_to_params() = 0;
        inline virtual double log_likelihood() const = 0;

        // This is the default naming scheme, but it may or may not be used
        static const ParameterNames parameter_names;
};

/* Implementations follow */

template<int num_params, typename T>
const ParameterNames UniformModel<num_params, T>::parameter_names(num_params);

template<int num_params, typename T>
inline UniformModel<num_params, T>::UniformModel(RNG& rng)
:us(num_params)
,xs(num_params)
{
    for(double& u: us)
        u = rng.rand();
}

template<int num_params, typename T>
inline double UniformModel<num_params, T>::perturb(RNG& rng)
{
    int num = 1;
    if(rng.rand() <= 0.5)
        num = int(pow(us.size(), rng.rand()));

    for(int i=0; i<num; ++i)
    {
        int k = rng.rand_int(us.size());
        us[k] += rng.randh();
        wrap(us[k]);
    }
    us_to_params();

    return 0.0;
}


template<int num_params, typename T>
inline std::vector<char> UniformModel<num_params, T>::to_blob() const
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


template<int num_params, typename T>
inline void UniformModel<num_params, T>::from_blob(const std::vector<char>& vec)
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

template<int num_params, typename T>
inline std::string UniformModel<num_params, T>::to_string() const
{
    std::stringstream ss;
    ss << std::setprecision(Options::stdout_precision);
    for(int i=0; i<num_params; ++i)
    {
        ss << xs[i];
        if(i < num_params - 1)
            ss << ',';
    }
    return ss.str();
}


template<int num_params, typename T>
inline double& UniformModel<num_params, T>::param(std::string&& name)
{
    return xs[T::parameter_names.index(std::move(name))];
}

template<int num_params, typename T>
inline const double& UniformModel<num_params, T>::param(std::string&& name) const
{
    return xs[T::parameter_names.index(std::move(name))];
}

} // namespace

#endif

