#include "CommandLineOptions.h"

namespace DNest5
{

	// The following code is based on the example given at
	// http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt

CommandLineOptions::CommandLineOptions(int argc, char** argv)
:temperature(1.0)
,abc(false)
,abc_fraction(0.8)
{
	int c;
	std::stringstream ss;

	opterr = 0;
	while((c = getopt(argc, argv, "haf:t:")) != -1)
    {
	    switch(c)
	    {
		    case 'h':
			    print_help();
			    break;
            case 'a':
                abc = true;
                break;
            case 'f':
                ss << optarg;
                ss >> abc_fraction;
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

    if(abc && temperature != 1.0)
        std::cerr << "Temperature option has no effect in ABC mode." << std::endl;

    if(!abc && abc_fraction != 0.8)
        std::cerr << "abc_fraction only has an effect in ABC mode." << std::endl;
}

void CommandLineOptions::print_help() const
{
    std::cout << "Command line options for postprocessing:" << std::endl;
    std::cout << "    -t <temperature>     (default=1.0)" << std::endl;
    std::cout << "    -a                   (ABC mode)" << std::endl;
    std::cout << "    -f <abc_fraction>    (default=0.8)" << std::endl;
    exit(0);
}

} // namespace

