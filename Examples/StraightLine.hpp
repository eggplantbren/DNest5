#ifndef DNest5_StraightLine_hpp
#define DNest5_StraightLine_hpp

#include "UniformModel.hpp"
#include <fstream>

namespace DNest5
{

// The "Curiously Recursive" template argument is so that the correct
// naming scheme is used.
class StraightLine : public UniformModel<3, StraightLine>
{
    private:

        // The data
        static std::vector<double> data_xs, data_ys;

    public:

        // Data loader
        inline static void load_data(const char* filename);

        // Naming scheme
        static const ParameterNames parameter_names;

        inline StraightLine(RNG& rng);
        inline void us_to_params();
        inline double log_likelihood() const;
};

/* Implementations follow */

// Data and its loading
std::vector<double> StraightLine::data_xs;
std::vector<double> StraightLine::data_ys;
inline void StraightLine::load_data(const char* filename)
{
    data_xs.clear();
    data_ys.clear();
    std::fstream fin(filename, std::ios::in);
    double x, y;
    while(fin >> x && fin >> y)
    {
        data_xs.push_back(x);
        data_ys.push_back(y);
    }
    fin.close();
}

// Define parameter names
const ParameterNames StraightLine::parameter_names({"m", "b", "sigma"});

inline StraightLine::StraightLine(RNG& rng)
:UniformModel(rng)
{
    us_to_params();
    if(data_xs.size() == 0)
        load_data("Examples/road.txt");
}

inline void StraightLine::us_to_params()
{
    param("m") = 1000.0*qnorm(us[0]);
    param("b") = 1000.0*qnorm(us[1]);
    param("sigma") = exp(-10.0 + 20.0*us[2]);
}

inline double StraightLine::log_likelihood() const
{
    double logl = 0.0;

    double var = pow(param("sigma"), 2);
    double tau = 1.0/var;
    double c = -0.5*log(2.0*M_PI*var);
    for(int i=0; i<int(data_xs.size()); ++i)
    {
        double mu = param("m")*data_xs[i] + param("b");
        logl += c - 0.5*tau*pow(data_ys[i] - mu, 2);
    }

    return logl;
}

} // namespace

#endif

