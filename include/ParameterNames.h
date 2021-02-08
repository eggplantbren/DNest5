#ifndef DNest5_ParameterNames_h
#define DNest5_ParameterNames_h

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
        inline int index(std::string&& name) const { return lookup.at(name); };

        // CSV header text
        std::string csv_header() const;
};

} // namespace

#endif

