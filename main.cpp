#include <iostream>
#include <DNest5/Sampler.hpp>
#include <ModelType.hpp>

using namespace DNest5;

int main()
{
    Sampler<ModelType> sampler(Options("options.yaml"));
    sampler.run();
    return 0;
}

