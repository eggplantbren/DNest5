#include <DNest5/CommandLineOptions.hpp>
#include <DNest5/Database.hpp>
#include "ModelType.hpp"

using namespace DNest5_Template;

int main(int argc, char** argv)
{
    DNest5::CommandLineOptions options(argc, argv);
    DNest5::postprocess<ModelType>(options);
    return 0;
}

