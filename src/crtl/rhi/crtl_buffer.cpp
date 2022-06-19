#pragma once

#include "crtl/crtl_buffer.h"

extern "C" CRTL_RHI_EXPORT CRTLBuffer crtl_allocate_buffer(CRTLDevice device,
                                                           CRTL_MEMORY_SPACE memory,
                                                           size_t size_bytes)
{
    // TODO: Here load the backend module and do a dlsym if it's not loaded already,
    // then create the device using the module
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT CRTLBufferView crtl_make_view(CRTLDevice device,
                                                         CRTLBuffer buffer,
                                                         CRTL_DATA_TYPE type,
                                                         size_t offset_bytes,
                                                         size_t n_elements)
{
    // TODO:
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void *crtl_map_view(CRTLDevice device, CRTLBufferView view)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void crtl_unmap_view(CRTLDevice device, CRTLBufferView view)
{
    // TODO
}
