#ifndef DNest5_UniformModel_hpp
#define DNest5_UniformModel_hpp

#include <cstring>
#include <DNest5/Misc.hpp>
#include <DNest5/NamingScheme.hpp>
#include <DNest5/Options.hpp>
#include <DNest5/RNG.hpp>
#include <map>
#include <sstream>
#include <string>

namespace DNest5
{

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

        // This is the default naming scheme, but it may or may not be used
        static const NamingScheme naming_scheme;

    public:

        // Default constructor sets up the vectors
        UniformModel(RNG& rng);
        virtual ~UniformModel() = default;

        // Functions specified here and not to be overridden
        double perturb(RNG& rng);
        std::vector<char> to_blob() const;
        void from_blob(const std::vector<char>& vec);
        std::string to_string() const;

        // Access parameters by name
        virtual double& param(std::string&& name);
        virtual const double& param(std::string&& name) const;

        // Functions to be specified in the derived class
        virtual void us_to_params() = 0;
        virtual double log_likelihood() const = 0;
};

/* Implementations follow */

template<int num_params, typename T>
const NamingScheme UniformModel<num_params, T>::naming_scheme(num_params);

template<int num_params, typename T>
UniformModel<num_params, T>::UniformModel(RNG& rng)
:us(num_params)
,xs(num_params)
{
    for(double& u: us)
        u = rng.rand();
}

template<int num_params, typename T>
double UniformModel<num_params, T>::perturb(RNG& rng)
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
std::vector<char> UniformModel<num_params, T>::to_blob() const
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
void UniformModel<num_params, T>::from_blob(const std::vector<char>& vec)
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
std::string UniformModel<num_params, T>::to_string() const
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
double& UniformModel<num_params, T>::param(std::string&& name)
{
    return xs[T::naming_scheme.index(std::move(name))];
}

template<int num_params, typename T>
const double& UniformModel<num_params, T>::param(std::string&& name) const
{
    return xs[T::naming_scheme.index(std::move(name))];
}

} // namespace

#endif

