#pragma once

#include "crtl/crtl_device.h"
#include "parallel_hashmap/phmap.h"

extern "C" CRTL_RHI_EXPORT CRTLDevice crtl_create_device(const DEVICE_API api)
{
    // TODO: Here load the backend module and do a dlsym if it's not loaded already,
    // then create the device using the module
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void crtl_retain(CRTLDevice device, CRTLAPIObject object)
{
    // TODO:
}

extern "C" CRTL_RHI_EXPORT void crtl_release(CRTLDevice device, CRTLAPIObject object)
{
    // TODO
}
