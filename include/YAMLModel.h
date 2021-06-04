#ifndef DNest5_YAMLModel_h
#define DNest5_YAMLModel_h

#include <yaml-cpp/yaml.h>

namespace DNest5
{

class YAMLModel
{
    private:
        YAML::Node node;

    public:
        YAMLModel() = delete;
        YAMLModel(const char* filename);
};

} // namespace

#endif
