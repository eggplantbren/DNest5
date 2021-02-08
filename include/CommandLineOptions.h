#ifndef DNest5_CommandLineOptions_h
#define DNest5_CommandLineOptions_h

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

namespace DNest5
{

/*
* Command Line Options for postprocessing
*/

class CommandLineOptions
{
	private:
        double temperature;
        bool abc;
        double abc_fraction;

	public:
        // Construct
		CommandLineOptions(int argc, char** argv);

		// Print help message
		void print_help() const;

        // Getters
        inline double get_temperature() const { return temperature; }
        inline bool get_abc() const { return abc; }
        inline double get_abc_fraction() const { return abc_fraction; }
};

} // namespace DNest5

#endif

