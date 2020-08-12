#ifndef DNest5_Misc_hpp
#define DNest5_Misc_hpp

#include <algorithm>
#include <boost/math/special_functions/erf.hpp>
#include <cassert>
#include <cmath>
#include <tuple>
#include <vector>

namespace DNest5
{

// Minus infinity
static constexpr double minus_infinity
                            = -std::numeric_limits<double>::infinity();

// Logsumexp etc
double logsumexp(const std::vector<double>& logv);
double logdiffexp(double b, double a);
void normalise_logps(std::vector<double>& logps);

// Modulo and related things
int mod(int y, int x);
void wrap(double& x, double min=0.0, double max=1.0);

// Special functions that are often useful
double qnorm(double p);



/* Implementations follow */

double logsumexp(const std::vector<double>& logv)
{
    double max = *max_element(logv.begin(), logv.end());
    double answer = 0.0;
    for(size_t i=0; i<logv.size(); ++i)
        answer += exp(logv[i] - max);
    answer = max + log(answer);
    return answer;
}

double logdiffexp(double b, double a)
{
    assert(b >= a);
    return b + log(1 - exp(a-b));
}

void normalise_logps(std::vector<double>& logps)
{
    double logz = logsumexp(logps);
    for(double& logp: logps)
        logp -= logz;
}

double mod(double y, double x)
{
    assert(x > 0.0);
    return (y/x - floor(y/x))*x;
}

void wrap(double& x, double min, double max)
{
    x = DNest5::mod(x - min, max - min) + min;
}

double qnorm(double p)
{
    return sqrt(2.0)*boost::math::erf_inv(2.0*p - 1.0);
}

} // namespace

#endif

