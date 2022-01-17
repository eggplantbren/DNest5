#include "CommandLineOptions.h"
#include "Postprocessing.h"
#include "ModelType.h"

using namespace DNest5;

int main(int argc, char** argv)
{
    CommandLineOptions options(argc, argv);
    postprocess<DNest5_Template::ModelType>(options);
    return 0;
}

