#include <iostream>
#include <DNest5/Sampler.hpp>
#include <DNest5/SpikeSlab.hpp>

using namespace DNest5;

int main()
{
    Sampler<SpikeSlab> sampler;
    sampler.run();
    return 0;
}

