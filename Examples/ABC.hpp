#ifndef DNest5_ABC_hpp
#define DNest5_ABC_hpp

#include <algorithm>
#include <cstring>
#include "ParameterNames.h"
#include <fstream>
#include <sstream>
#include <string>
#include <Tools/Misc.hpp>
#include <Tools/RNG.hpp>
#include <vector>

namespace DNest5
{

using Tools::RNG, Tools::wrap;

class ABC
{
    private:
        double mu;
        double sigma;
        std::vector<double> ns;

	public:

        inline ABC(RNG& rng);
        inline double perturb(RNG& rng);
        inline double log_likelihood() const;
        inline std::vector<char> to_blob() const;
        inline void from_blob(const std::vector<char>& vec);
        inline std::string to_string() const;


    /* Static things */
	private:

        // The data
        static std::vector<double> data_xs;

    public:

        // The parameter names
        static ParameterNames parameter_names;

        // Data loader
        inline static void load_data(const char* filename);
};

/* IMPLEMENTATIONS FOLLOW */

std::vector<double> ABC::data_xs;
ParameterNames ABC::parameter_names(0);

inline void ABC::load_data(const char* filename)
{
    data_xs.clear();
    std::fstream fin(filename, std::ios::in);
    double x;
    while(fin >> x)
        data_xs.push_back(x);
    fin.close();
    std::cout << "Loaded " << data_xs.size() << " data points." << std::endl;

    // Set parameter names
    std::vector<std::string> names = {"mu", "sigma"};
    for(size_t i=0; i<data_xs.size(); ++i)
    {
        std::stringstream ss;
        ss << "n[" << i << ']';
        names.emplace_back(ss.str());
    }
    parameter_names = ParameterNames(names);
}

inline ABC::ABC(RNG& rng)
{
    if(data_xs.size() == 0)
        load_data("Examples/abc_data.txt");

    mu = -10.0 + 20.0*rng.rand();
    sigma = exp(-10.0 + 20.0*rng.rand());

    ns.resize(data_xs.size());
    for(double& n: ns)
        n = rng.randn();
}

inline double ABC::perturb(RNG& rng)
{
    double logh = 0.0;

    if(rng.rand() <= 0.5)
    {
        int which = rng.rand_int(2);
        if(which == 0)
        {
            mu += 20.0*rng.randh();
            wrap(mu, -10.0, 10.0);
        }
        else
        {
            sigma = log(sigma);
            sigma += 20.0*rng.randh();
            wrap(sigma, -10.0, 10.0);
            sigma = exp(sigma);
        }
    }
    else
    {
        int which = rng.rand_int(ns.size());
        logh -= -0.5*pow(ns[which], 2);
        ns[which] += rng.randh();
        logh += -0.5*pow(ns[which], 2);
    }

    return logh;
}

inline double ABC::log_likelihood() const
{
    double logl = 0.0;

    // Compute fake dataset
    std::vector<double> xs(ns.size());
    for(size_t i=0; i<ns.size(); ++i)
        xs[i] = mu + sigma*ns[i];

    // Real min and max
    double xmin = *min_element(data_xs.begin(), data_xs.end());
    double xmax = *max_element(data_xs.begin(), data_xs.end());
    logl += -std::abs(xmin - *min_element(xs.begin(), xs.end()));
    logl += -std::abs(xmax - *max_element(xs.begin(), xs.end()));

    return logl;
}

inline void ABC::from_blob(const std::vector<char>& vec)
{
    int chars_per_element = sizeof(double)/sizeof(char);

    std::memcpy(&mu, &vec[0], sizeof(double));
    std::memcpy(&sigma, &vec[chars_per_element], sizeof(double));
    int j = 2*chars_per_element;
    for(size_t i=0; i<ns.size(); ++i)
    {
        std::memcpy(&ns[i], &vec[j], sizeof(double));
        j += chars_per_element;
    }
}

inline std::vector<char> ABC::to_blob() const
{
    int chars_per_element = sizeof(double)/sizeof(char);
    int chars_needed = (2 + ns.size())*chars_per_element;
    std::vector<char> result(chars_needed);

    int j = 0;
    std::memcpy(&result[j], &mu, sizeof(double));
    j += chars_per_element;
    std::memcpy(&result[j], &sigma, sizeof(double));
    j += chars_per_element;

    for(size_t i=0; i<ns.size(); ++i)
    {
        std::memcpy(&result[j], &ns[i], sizeof(double));
        j += chars_per_element;
    }
    return result;
}

inline std::string ABC::to_string() const
{
    std::stringstream ss;
    ss << mu << ',' << sigma << ',';
    for(int i=0; i<int(ns.size()); ++i)
    {
        ss << ns[i];
        if(i != int(ns.size())-1)
            ss << ',';
    }
    return ss.str();
}


} // namespace

#endif

