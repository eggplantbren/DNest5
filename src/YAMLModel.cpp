#include "YAMLModel.h"

namespace DNest5
{

YAMLModel::YAMLModel(const char* filename)
:node{YAML::LoadFile(filename)}
{

}


} // namespace
