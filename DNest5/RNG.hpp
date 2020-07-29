#ifndef DNest5_RNG_hpp
#define DNest5_RNG_hpp

#include <cmath>
#include <cstring>
#include <DNest5/Skilling.hpp>
#include <vector>

namespace DNest5
{

/*
* An object of this class represents a random number generator state
*/
class RNG
{
    private:

        // One of John's things
        Rand64_t skilling_rng;

    public:

        // Default constructor
        RNG();

        // Set the seed
        void set_seed(int seed);

        // Uniform(0, 1)
        double rand();

        // Normal(0, 1)
        double randn();

        // A heavy-tailed distribution for proposals
        double randh();

        // Cauchy
        double randc();

        // Integer from {0, 1, 2, ..., N-1}
        int rand_int(int N);

        // State to blob
        std::vector<char> to_blob() const;

}; // class RNG


/* IMPLEMENTATION FOLLOWS */

RNG::RNG()
{

}

void RNG::set_seed(int seed)
{
    Ran64Init(skilling_rng, seed);
}

double RNG::rand()
{
    return RanDouble(skilling_rng);
}

double RNG::randn()
{
    return RanGauss(skilling_rng);
}

double RNG::randh()
{
    return pow(10.0, 1.0 - std::abs(this->randc()))*this->randn();
}

double RNG::randc()
{
    return tan(M_PI*(this->rand() - 0.5));
}

int RNG::rand_int(int N)
{
    return static_cast<int>(floor(N*this->rand()));
}

std::vector<char> RNG::to_blob() const
{
    int chars_per_element = sizeof(unsigned long)/sizeof(char);
    int chars_needed = 4*chars_per_element;
    std::vector<char> result(chars_needed);
    int j = 0;
    for(int i=0; i<4; ++i)
    {
        std::memcpy(&result[j], &(skilling_rng[i]), sizeof(unsigned long));
        j += chars_per_element;
    }
    return result;
}

} // namespace

#endif

