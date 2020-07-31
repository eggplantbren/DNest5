#ifndef DNest5_Barrier_hpp
#define DNest5_Barrier_hpp

#include <mutex>
#include <condition_variable>

namespace DNest5
{

/*
* Barrier class based on an answer at
* http://stackoverflow.com/questions/24465533/implementing-boostbarrier-in-c11
*/

class Barrier
{
	private:
		std::mutex the_mutex;
		std::condition_variable cond;
		unsigned int threshold;
		unsigned int count;
		unsigned int generation;

	public:
		// Constructor: initialise count
		explicit Barrier(unsigned int count);

		// Wait method
		void wait();
};

/* Implementation follows */

Barrier::Barrier(unsigned int count)
:threshold(count)
,count(count)
,generation(0)
{

}

void Barrier::wait()
{
	unsigned int gen = generation;
	std::unique_lock<std::mutex> lock{the_mutex};
	if((--count) == 0)
	{
		generation++;
		count = threshold;
		cond.notify_all();
	}
	else
		cond.wait(lock, [this, gen] { return gen != generation; });
}

} // namespace DNest5

#endif

