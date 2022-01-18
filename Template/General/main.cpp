#include <iostream>
#include <Misc.h>
#include <Sampler.hpp>
#include "ModelType.h"

using DNest5::Options, DNest5::Sampler;

int main()
{
    // Tester for to/from blob
//    using T = DNest5_Template::ModelType;
//    Tools::RNG rng;
//    T t(rng);
//    std::cout << t.to_string() << std::endl;
//    auto blob = t.to_blob();
//    t.from_blob(blob);
//    std::cout << t.to_string() << std::endl;

    DNest5::clear_output_dir();
    Sampler<DNest5_Template::ModelType> sampler(Options("options.yaml"));
    sampler.run();
    return 0;
}

