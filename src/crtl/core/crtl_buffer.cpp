#include "crtl/crtl_buffer.h"
#include "device.h"

extern "C" CRTL_EXPORT CRTL_ERROR crtl_new_buffer(CRTLDevice device,
                                                  CRTL_MEMORY_SPACE memory_space,
                                                  CRTL_BUFFER_USAGE usages,
                                                  size_t size_bytes,
                                                  CRTLBuffer *buffer)
{
    if (size_bytes == 0) {
        return CRTL_ERROR_INVALID_BUFFER_SIZE;
    }
    // TODO: need to filter invalid memory space/buffer usage combinations
    // Note: map write with readback and map read with upload heap should also be
    // considered invalid, device heap cannot be mapped,a buffer cannot be map read & map
    // write
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_buffer(memory_space, usages, size_bytes, buffer);
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
                                                       CRTL_BUFFER_MAP_MODE map_mode,
                                                       void **mapping)
{
    // TODO: Buffer usage and map mode validation, cannot map a buffer that's not map
    // write for writing, can't map a non-map read one for reading
    // also have to check mapping isn't null
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->map_buffer_view(view, map_mode, mapping);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_unmap_buffer_view(CRTLDevice device,
                                                         CRTLBufferView view)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->unmap_buffer_view(view);
}
