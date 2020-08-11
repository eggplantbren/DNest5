#ifndef DNest5_ParameterNames_hpp
#define DNest5_ParameterNames_hpp

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace DNest5
{

/* A naming scheme for the parameters of a UniformModel */

class ParameterNames
{
    private:

        int num_params;
        std::vector<std::string> names;
        std::map<std::string, int> lookup;

    public:

        // Create a default naming scheme
        ParameterNames(int _num_params);

        // Create a naming scheme with the given names
        ParameterNames(const std::vector<std::string>& _names);

        // Get index
        int index(std::string&& name) const { return lookup.at(name); };

        // CSV header text
        std::string csv_header() const;
};

/* IMPLEMENTATIONS FOLLOW */

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

#endif
