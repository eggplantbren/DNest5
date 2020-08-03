#ifndef DNest5_StraightLine_hpp
#define DNest5_StraightLine_hpp

#include <boost/math/special_functions/erf.hpp>
#include <DNest5/UniformModel.hpp>
#include <fstream>

namespace DNest5
{

class StraightLine:public UniformModel<3>
{
    private:

        // The data
        static std::vector<double> data_xs, data_ys;

        // Quantile function of standard normal
        static double qnorm(double p);

    public:

        // Data loader
        static void load_data(const char* filename);

        StraightLine(RNG& rng);
        void us_to_params();
        double log_likelihood() const;
};

/* Implementations follow */

std::vector<double> StraightLine::data_xs;
std::vector<double> StraightLine::data_ys;
void StraightLine::load_data(const char* filename)
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
    std::cout << "Loaded " << data_xs.size() << " data points from ";
    std::cout << filename << "." << std::endl;
}


StraightLine::StraightLine(RNG& rng)
:UniformModel(rng)
{
    us_to_params();
}

void StraightLine::us_to_params()
{
    params["m"] = 1000.0*qnorm(us[0]);
    params["b"] = 1000.0*qnorm(us[1]);
    params["sigma"] = exp(5.0*us[2]);
}

double StraightLine::qnorm(double p)
{
    return sqrt(2.0)*boost::math::erf_inv(2.0*p - 1.0);
}

double StraightLine::log_likelihood() const
{
    double logl = 0.0;

    double var = pow(params.at("sigma"), 2);
    double tau = 1.0/var;
    double c = -0.5*log(2.0*M_PI*var);
    for(int i=0; i<int(data_xs.size()); ++i)
    {
        double mu = params.at("m")*data_xs[i] + params.at("b");
        logl += c - 0.5*tau*pow(data_ys[i] - mu, 2);
    }

    return logl;
}

} // namespace

#endif

