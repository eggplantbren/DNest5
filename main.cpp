#include <iostream>
#include "Examples/StraightLine.hpp"
#include "Misc.h"
#include "Sampler.hpp"

using namespace DNest5;

int main()
{
    clear_output_dir();
    Sampler<StraightLine> sampler(Options("options.yaml"));
    sampler.run();
    return 0;
}

