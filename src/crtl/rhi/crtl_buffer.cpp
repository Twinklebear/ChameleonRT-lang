#include "crtl/crtl_buffer.h"
#include "device.h"

extern "C" CRTL_RHI_EXPORT CRTLBuffer crtl_new_buffer(CRTLDevice device,
                                                      CRTL_MEMORY_SPACE memory,
                                                      CRTL_BUFFER_USAGE usages,
                                                      size_t size_bytes)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_buffer(memory, usages, size_bytes);
}

extern "C" CRTL_RHI_EXPORT CRTLBufferView crtl_new_view(CRTLDevice device,
                                                        CRTLBuffer buffer,
                                                        CRTL_DATA_TYPE type,
                                                        size_t offset_bytes,
                                                        size_t n_elements)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_view(buffer, type, offset_bytes, n_elements);
}

extern "C" CRTL_RHI_EXPORT void *crtl_map_view(CRTLDevice device, CRTLBufferView view)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->map_view(view);
}

extern "C" CRTL_RHI_EXPORT void crtl_unmap_view(CRTLDevice device, CRTLBufferView view)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->unmap_view(view);
}
