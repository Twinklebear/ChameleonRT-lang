#include "crtl/crtl_parameter_block.h"
#include "device.h"

extern "C" CRTL_RHI_EXPORT void crtl_set_parameter(CRTLDevice device,
                                                   CRTLParameterBlock parameter_block,
                                                   const char *name,
                                                   CRTL_DATA_TYPE data_type,
                                                   void *parameter)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->set_parameter(parameter_block, name, data_type, parameter);
}
