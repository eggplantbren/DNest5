#include <iostream>
#include <Misc.h>
#include <Sampler.hpp>
#include "ModelType.h"

using DNest5::Options, DNest5::Sampler;

int main()
{
    DNest5::clear_output_dir();
    Sampler<DNest5_Template::ModelType> sampler(Options("options.yaml"));
    sampler.run();
    return 0;
}

