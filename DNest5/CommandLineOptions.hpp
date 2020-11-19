#ifndef DNest5_CommandLineOptions_hpp
#define DNest5_CommandLineOptions_hpp

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

namespace DNest5
{

/*
* A little class containing all of the command line options.
* Uses GNU GetOpt to parse the input.
*/

class CommandLineOptions
{
	private:
        double temperature;

	public:
        // Construct
		CommandLineOptions(int argc, char** argv);

		// Print help message
		void print_help() const;

        // Getters
        double get_temperature() const { return temperature; }

};


/* IMPLEMENTATIONS FOLLOW */

	// The following code is based on the example given at
	// http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt

CommandLineOptions::CommandLineOptions(int argc, char** argv)
:temperature(1.0)
{
	int c;
	std::stringstream ss;

	opterr = 0;
	while((c = getopt(argc, argv, "ht:")) != -1)
    {
	    switch(c)
	    {
		    case 'h':
			    print_help();
			    break;
            case 't':
                ss << optarg;
                ss >> temperature;
                break;
		    case '?':
			    std::cerr << "# Option "<< optopt <<" requires an argument." <<std::endl;
			    if(isprint(optopt))
				    std::cerr << "# Unknown option " << optopt << "." << std::endl;
			    else
				    std::cerr << "# Unknown option character " << optopt << "." <<std::endl;
			    exit(-1);
		    default:
			    abort();
        }
    }

	for(int index = optind; index < argc; index++)
		std::cerr << "# Non-option argument " << argv[index] << std::endl;

}

void CommandLineOptions::print_help() const
{
    std::cout << "Command line options for postprocessing:" << std::endl;
    std::cout << "-t <temperature>     (default=1.0)" << std::endl;
    exit(0);
}

} // namespace DNest5

#endif

