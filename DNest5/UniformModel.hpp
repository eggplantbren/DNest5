#ifndef DNest5_UniformModel_hpp
#define DNest5_UniformModel_hpp

#include <cstring>
#include <DNest5/Misc.hpp>
#include <DNest5/RNG.hpp>
#include <map>
#include <string>

namespace DNest5
{

/*
    Derive from this class to implement models using an underlying
    set of coordinates with Uniform(0, 1) priors.
*/
template<int num_params>
class UniformModel
{
    protected:

        // Underlying coordinates and their transformed version
        std::vector<double> us;
        std::vector<double> xs;

        // As an alternative to xs, here is a map from strings
        // (parameter names) to values (parameter values). Use one or the other.
        std::map<std::string, double> params;

    public:

        // Default constructor sets up the vectors
        UniformModel(RNG& rng);
        virtual ~UniformModel();

        // Functions specified here
        virtual double perturb(RNG& rng) final;
        std::vector<char> to_blob() const;
        void from_blob(const std::vector<char>& vec);

        // Functions to be specified in the derived class
        virtual void us_to_params() = 0;
        virtual double log_likelihood() const = 0;
};

/* Implementations follow */

template<int num_params>
UniformModel<num_params>::UniformModel(RNG& rng)
:us(num_params)
,xs(num_params)
{
    for(double& u: us)
        u = rng.rand();
}


template<int num_params>
UniformModel<num_params>::~UniformModel()
{

}

template<int num_params>
double UniformModel<num_params>::perturb(RNG& rng)
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


template<int num_params>
std::vector<char> UniformModel<num_params>::to_blob() const
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


template<int num_params>
void UniformModel<num_params>::from_blob(const std::vector<char>& vec)
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

