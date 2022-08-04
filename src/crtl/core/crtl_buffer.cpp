#include "crtl/crtl_buffer.h"
#include "device.h"

extern "C" CRTL_EXPORT CRTL_ERROR crtl_new_buffer(CRTLDevice device,
                                                  CRTL_MEMORY_SPACE memory,
                                                  CRTL_BUFFER_USAGE usages,
                                                  size_t size_bytes,
                                                  CRTLBuffer *buffer)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_buffer(memory, usages, size_bytes, buffer);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_new_buffer_view(CRTLDevice device,
                                                       CRTLBuffer buffer,
                                                       CRTL_DATA_TYPE type,
                                                       size_t offset_bytes,
                                                       size_t n_elements,
                                                       CRTLBufferView *view)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_buffer_view(buffer, type, offset_bytes, n_elements, view);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_map_buffer_view(CRTLDevice device,
                                                       CRTLBufferView view,
                                                       void **mapping)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->map_buffer_view(view, mapping);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_unmap_buffer_view(CRTLDevice device,
                                                         CRTLBufferView view)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->unmap_buffer_view(view);
}
