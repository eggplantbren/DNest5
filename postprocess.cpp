#include <DNest5/CommandLineOptions.hpp>
#include <DNest5/Database.hpp>
#include <ModelType.hpp>

using namespace DNest5;

int main(int argc, char** argv)
{
    CommandLineOptions options(argc, argv);
    postprocess<ModelType>(options);
    return 0;
}

