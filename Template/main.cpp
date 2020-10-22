#include <iostream>
#include <DNest5/Sampler.hpp>
#include "ModelType.hpp"

using namespace DNest5_Template;
using DNest5::Options, DNest5::Sampler;

int main()
{
    Sampler<ModelType> sampler(Options("options.yaml"));
    sampler.run();
    return 0;
}

