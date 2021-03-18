#ifndef DNest5_ParameterNames_h
#define DNest5_ParameterNames_h

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace DNest5
{

/* A naming scheme for the parameters of a model */

class ParameterNames
{
    private:

        int num_params;
        std::vector<std::string> names;
        std::map<std::string, int> lookup;

    public:

        // Empty
        ParameterNames();

        // Create a default naming scheme
        ParameterNames(int _num_params);

        // Create a naming scheme with the given names
        ParameterNames(const std::vector<std::string>& _names);

        // Add one
        void add(std::string name);

        // Get index
        inline int index(std::string&& name) const { return lookup.at(name); };

        // Get size
        inline int size() const { return num_params; };

        // CSV header text
        std::string csv_header() const;
};

} // namespace

#endif

