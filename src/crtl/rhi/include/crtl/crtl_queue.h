#pragma once

#include <stdint.h>
#include "crtl_buffer.h"
#include "crtl_device.h"
#include "crtl_event.h"
#include "crtl_geometry.h"
#include "crtl_parameter_block.h"
#include "crtl_rtpipeline.h"
#include "crtl_texture.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct Queue : APIObject {};
struct CommandAllocator : APIObject {};
struct CommandBuffer : APIObject {};
}
typedef crtl_rhi::Queue *CRTLQueue;
typedef crtl_rhi::CommandAllocator *CRTLCommandAllocator;
typedef crtl_rhi::CommandBuffer *CRTLCommandBuffer;
#else
typedef CRTLAPIObject CRTLQueue;
typedef CRTLAPIObject CRTLCommandAllocator;
typedef CRTLAPIObject CRTLCommandBuffer;
#endif

typedef struct CRTLBox3D {
    uint32_t offset_x;
    uint32_t offset_y;
    uint32_t offset_z;
    uint32_t size_x;
    uint32_t size_y;
    uint32_t size_z;
} CRTLBox3D;

#ifdef __cplusplus
extern "C" {
#endif

/* Create a new queue to submit commands to the GPU
 */
CRTL_RHI_EXPORT CRTLQueue crtl_new_queue(CRTLDevice device);

/* Create a new command allocator that can be shared by command encoders to encode
 * rendering commands
 * Will note: here the API is a bit in between WebGPU and DX12/Vulkan, WebGPU doesn't
 * split the command allocator out from the Command Encoder while DX12/Vulkan do expose
 * this detail, and I think it can be useful to have that lower level of control
 */
CRTL_RHI_EXPORT CRTLCommandAllocator crtl_new_command_allocator(CRTLDevice device,
                                                                CRTLQueue queue);

/* Reset all commands allocated from this command allocator, all command encoders/buffers
 * will be invalid
 */
CRTL_RHI_EXPORT void crtl_reset_command_allocator(CRTLDevice device,
                                                  CRTLCommandAllocator cmd_allocator);

CRTL_RHI_EXPORT CRTLCommandBuffer
crtl_new_command_buffer(CRTLDevice device, CRTLCommandAllocator cmd_allocator);

CRTL_RHI_EXPORT void crtl_close_command_buffer(CRTLDevice device,
                                               CRTLCommandBuffer cmd_buffer);

CRTL_RHI_EXPORT CRTLEvent crtl_submit_command_buffer(CRTLDevice device,
                                                     CRTLQueue queue,
                                                     CRTLCommandBuffer cmd_buffer);

CRTL_RHI_EXPORT void crtl_copy_buffer_to_buffer(CRTLDevice device,
                                                CRTLCommandBuffer cmd_buffer,
                                                CRTLBuffer src,
                                                uint64_t src_offset,
                                                CRTLBuffer dst,
                                                uint64_t dst_offset,
                                                uint64_t size);

CRTL_RHI_EXPORT void crtl_copy_buffer_to_texture(CRTLDevice device,
                                                 CRTLCommandBuffer cmd_buffer,
                                                 CRTLBuffer src,
                                                 uint64_t src_offset,
                                                 CRTLTexture dst,
                                                 CRTLBox3D region);

CRTL_RHI_EXPORT void crtl_copy_texture_to_buffer(CRTLDevice device,
                                                 CRTLCommandBuffer cmd_buffer,
                                                 CRTLTexture src,
                                                 CRTLBox3D region,
                                                 CRTLBuffer dst,
                                                 uint64_t dst_offset);

/* TODO/Note to include: build and compaction cannot be done in the same command buffer
 * since we need to readback the compact size and allocate room for it
 */
CRTL_RHI_EXPORT void crtl_build_blas(CRTLDevice device,
                                     CRTLCommandBuffer cmd_buffer,
                                     CRTLGroup group);

CRTL_RHI_EXPORT void crtl_compact_blas(CRTLDevice device,
                                       CRTLCommandBuffer cmd_buffer,
                                       CRTLGroup group);

CRTL_RHI_EXPORT void crtl_build_tlas(CRTLDevice device,
                                     CRTLCommandBuffer cmd_buffer,
                                     CRTLScene scene);

CRTL_RHI_EXPORT void crtl_upload_shader_table(CRTLDevice device,
                                              CRTLCommandBuffer cmd_buffer,
                                              CRTLRTPipeline pipeline);

CRTL_RHI_EXPORT void crtl_set_rtpipeline(CRTLDevice device,
                                         CRTLCommandBuffer cmd_buffer,
                                         CRTLRTPipeline pipeline);

CRTL_RHI_EXPORT void crtl_set_global_parameter_block(
    CRTLDevice device,
    CRTLCommandBuffer cmd_buffer,
    CRTLGlobalParameterBlock parameter_block);

CRTL_RHI_EXPORT void crtl_dispatch_rays(CRTLDevice device,
                                        CRTLCommandBuffer cmd_buffer,
                                        uint32_t width,
                                        uint32_t height);

// TODO: Need APIs for barrier, resource transitions, etc. to match up with D3D12 & Vulkan
// At least do need barriers, might be possible to do the resource transitions internally
// but it needs knowledge of how the objects are used in the shaders, what parameters
// things are mapping too etc, might require tracking too much state to do well

#ifdef __cplusplus
}
#endif
