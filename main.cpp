#include <iostream>
#include "Examples/StraightLine.hpp"
#include "Sampler.hpp"

using namespace DNest5;

int main()
{
    Sampler<StraightLine> sampler(Options("options.yaml"));
    sampler.run();
    return 0;
}

