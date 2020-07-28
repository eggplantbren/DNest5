#ifndef DNest5_Particle_hpp
#define DNest5_Particle_hpp

#include <tuple>

namespace DNest5
{

// A particle is its parameters, logl, and tb, tupled together.
template<typename T>
using Particle = std::tuple<T, double, double>;

// One can extract the last two elements only
template<typename T>
std::tuple<double, double> logl_tb(const Particle<T>& particle);

// One can order (logl, tb) pairs
bool operator < (const std::tuple<double, double>& logl_tb1,
                 const std::tuple<double, double>& logl_tb2);

// One can order particles (by extracting the logl, tb pairs)
template<typename T>
bool operator < (const Particle<T>& x, const Particle<T>& y);



/* IMPLEMENTATIONS FOLLOW */

template<typename T>
std::tuple<double, double> logl_tb(const Particle<T>& particle)
{
    const auto& [t, logl, tb] = particle;
    return {logl, tb};
}

bool operator < (const std::tuple<double, double>& logl_tb1,
                 const std::tuple<double, double>& logl_tb2)
{
    // Unpack
    const auto& [logl1, tb1] = logl_tb1;
    const auto& [logl2, tb2] = logl_tb2;

    // Compare
    if(logl1 < logl2)
        return true;
    if(logl1 == logl2 && tb1 < tb2)
        return true;
    return false;
}


template<typename T>
bool operator < (const Particle<T>& x, const Particle<T>& y)
{
    return logl_tb(x) < logl_tb(y);
}

} // namespace

#endif

