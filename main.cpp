#include <iostream>
#include "Misc.h"
#include "ModelType.h"
#include "Sampler.hpp"

using namespace DNest5;

int main()
{
    clear_output_dir();
    Sampler<ModelType> sampler(Options("options.yaml"));
    sampler.run();
    return 0;
}

