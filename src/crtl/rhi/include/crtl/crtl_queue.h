#pragma once

#include <stdint.h>
#include "crtl_buffer.h"
#include "crtl_device.h"
#include "crtl_event.h"
#include "crtl_geometry.h"
#include "crtl_parameter_block.h"
#include "crtl_rtpipeline.h"

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

#ifdef __cplusplus
extern "C" {
#endif

/* Create a new queue to submit commands to the GPU
 */
CRTL_RHI_EXPORT CRTLQueue crtl_new_queue(CRTLDevice device);

CRTL_RHI_EXPORT CRTLEvent crtl_submit_command_buffer(CRTLDevice device,
                                                     CRTLCommandBuffer cmd_buffer);

// TODO: Also need a command encoder type API to push use of indirect command
// buffers, this should have API for buffer copies, texture copies, barriers, etc.
// then queue takes and runs sets of command buffers. Similar to WebGPU essentially

/* Create a new command allocator that can be shared by command encoders to encode
 * rendering commands
 * Will note: here the API is a bit in between WebGPU and DX12/Vulkan, WebGPU doesn't
 * split the command allocator out from the Command Encoder while DX12/Vulkan do expose
 * this detail
 */
CRTL_RHI_EXPORT CRTLCommandAllocator crtl_new_command_allocator(CRTLDevice device,
                                                                CRTLQueue queue);

/* Reset all commands allocated from this command allocator, all command encoders/buffers
 * will be invalid
 */
CRTL_RHI_EXPORT CRTLCommandAllocator
crtl_reset_command_allocator(CRTLDevice device, CRTLCommandAllocator cmd_allocator);

CRTL_RHI_EXPORT CRTLCommandBuffer
crtl_new_command_buffer(CRTLDevice device, CRTLCommandAllocator cmd_allocator);

CRTL_RHI_EXPORT void crtl_close_command_buffer(CRTLDevice device,
                                               CRTLCommandBuffer cmd_buffer);

CRTL_RHI_EXPORT void crtl_copy_buffer_to_buffer(CRTLDevice device,
                                                CRTLCommandBuffer cmd_buffer,
                                                CRTLBuffer src,
                                                uint64_t src_offset,
                                                CRTLBuffer dst,
                                                uint64_t dst_offset,
                                                uint64_t size);

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

#ifdef __cplusplus
}
#endif
