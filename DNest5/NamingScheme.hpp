#ifndef DNest5_NamingScheme_hpp
#define DNest5_NamingScheme_hpp

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace DNest5
{

/* A naming scheme for the parameters of a UniformModel */

class NamingScheme
{
    private:

        int num_params;
        std::vector<std::string> names;
        std::map<std::string, int> lookup;

    public:

        // Create a default naming scheme
        NamingScheme(int _num_params);

        // Create a naming scheme with the given names
        NamingScheme(const std::vector<std::string>& _names);

        // Get index
        int index(std::string&& name) const { return lookup.at(name); };
};

/* IMPLEMENTATIONS FOLLOW */

NamingScheme::NamingScheme(int _num_params)
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

NamingScheme::NamingScheme(const std::vector<std::string>& _names)
:num_params(_names.size())
,names(_names)
{
    for(int i=0; i<num_params; ++i)
        lookup.emplace(names[i], i);
}

} // namespace

#endif

