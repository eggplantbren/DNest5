#include "Particle.h"

namespace DNest5
{


bool operator < (const Pair& logl_tb1, const Pair& logl_tb2)
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

} // namespace
