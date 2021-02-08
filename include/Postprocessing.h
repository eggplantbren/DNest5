#ifndef DNest5_Postprocessing_h
#define DNest5_Postprocessing_h

#include "CommandLineOptions.h"

namespace DNest5
{

template<typename T>
inline void postprocess(const CommandLineOptions& options);

} // namespace

#endif

#include "PostprocessingImpl.h"

