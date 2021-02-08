#include "ParameterNames.h"

namespace DNest5
{



ParameterNames::ParameterNames(int _num_params)
:num_params(_num_params)
{
    names.reserve(num_params);
    for(int i=0; i<num_params; ++i)
    {
        std::stringstream ss;
        ss << "xs[" << i << "]";
        names.emplace_back(ss.str());
        lookup.emplace(ss.str(), i);
    }
}

ParameterNames::ParameterNames(const std::vector<std::string>& _names)
:num_params(_names.size())
,names(_names)
{
    for(int i=0; i<num_params; ++i)
        lookup.emplace(names[i], i);
}

std::string ParameterNames::csv_header() const
{
    std::stringstream ss;
    for(int i=0; i<int(names.size()); ++i)
    {
        ss << names[i];
        if(i < int(names.size())-1)
            ss << ',';
    }
    return ss.str();
}


} // namespace

