#ifndef DNest5_Particle_h
#define DNest5_Particle_h

#include <tuple>

namespace DNest5
{

// A particle is its parameters, logl, tb, and level j, tupled together.
template<typename T>
using Particle = std::tuple<T, double, double, int>;

// A pair
using Pair = std::tuple<double, double>;

// One can extract the last two elements only, yielding a Pair
template<typename T>
inline Pair logl_tb(const Particle<T>& particle);

// One can order (logl, tb) pairs
bool operator < (const Pair& logl_tb1, const Pair& logl_tb2);

// One can order particles (by extracting the logl, tb pairs)
template<typename T>
inline bool operator < (const Particle<T>& x, const Particle<T>& y);



/* TEMPLATE IMPLEMENTATIONS FOLLOW */

template<typename T>
inline Pair logl_tb(const Particle<T>& particle)
{
    const auto& [t, logl, tb, level] = particle;
    return {logl, tb};
}

template<typename T>
inline bool operator < (const Particle<T>& x, const Particle<T>& y)
{
    return logl_tb(x) < logl_tb(y);
}

} // namespace

#endif

