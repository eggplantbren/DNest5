#ifndef DNest5_PythonModel_hpp
#define DNest5_PythonModel_hpp

#include <boost/python.hpp>
#include <DNest5/UniformModel.hpp>

namespace DNest5
{

template<int num_params>
class PythonModel : public UniformModel<num_params, PythonModel<num_params>>
{
    private:


    public:

};

} // namespace

#endif

