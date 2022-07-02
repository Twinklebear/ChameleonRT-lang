#pragma once

#include "crtl_device.h"
#include "crtl_enums.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct Buffer : APIObject {};
struct BufferView : APIObject {};
}
typedef crtl_rhi::Buffer *CRTLBuffer;
typedef crtl_rhi::BufferView *CRTLBufferView;
#else
typedef CRTLAPIObject CRTLBuffer;
typedef CRTLAPIObject CRTLBufferView;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Allocate a buffer of the specified size in the desired memory location.
 * Buffers represent untyped blocks of device or host memory.
 * TODO: May need read/write flag or other usage flags here as well?
 * Or just need those for transitioning the resource using barrier
 */
CRTL_RHI_EXPORT CRTLBuffer crtl_new_buffer(CRTLDevice device,
                                           CRTL_MEMORY_SPACE memory,
                                           size_t size_bytes);

/* Create a typed view of the specified buffer. The view can be created at a desired offset in
 * bytes from the start of the buffer, and will contain n_elements elements of the specified
 * type. Strides of views must always be compact
 *
 * TODO: struct views should also be supported but the backend needs to dictate the struct
 * size and stride in the view, because it may have to be padded or aligned in some way for the
 * target API? What needs to be avoided is making a buffer with some stride between the
 * primitive types, because that would need additional handling in the shader to support
 * accessing it.
 * If all backends can be made to use a C struct layout that would be best
 * Vulkan can (scalar layout extension)
 * ISPC: maybe if can use pragma pack? Or just avoid using the builtin vec types.
 * DX12: probably?
 * CUDA: I think it is by default
 * Metal: Also might be by default?
 */
CRTL_RHI_EXPORT CRTLBufferView crtl_new_view(CRTLDevice device,
                                             CRTLBuffer buffer,
                                             CRTL_DATA_TYPE type,
                                             size_t offset_bytes,
                                             size_t n_elements);

CRTL_RHI_EXPORT void *crtl_map_view(CRTLDevice device, CRTLBufferView view);

CRTL_RHI_EXPORT void crtl_unmap_view(CRTLDevice device, CRTLBufferView view);

#ifdef __cplusplus
}
#endif
