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
CRTL_RHI_EXPORT CRTL_ERROR crtl_new_queue(CRTLDevice device, CRTLQueue *queue);

/* Create a new command allocator that can be shared by command encoders to encode
 * rendering commands
 * Will note: here the API is a bit in between WebGPU and DX12/Vulkan, WebGPU doesn't
 * split the command allocator out from the Command Encoder while DX12/Vulkan do expose
 * this detail, and I think it can be useful to have that lower level of control
 */
CRTL_RHI_EXPORT CRTL_ERROR crtl_new_command_allocator(
    CRTLDevice device, CRTLQueue queue, CRTLCommandAllocator *cmd_allocator);

/* Reset all commands allocated from this command allocator, all command encoders/buffers
 * will be invalid
 */
CRTL_RHI_EXPORT CRTL_ERROR
crtl_reset_command_allocator(CRTLDevice device, CRTLCommandAllocator cmd_allocator);

CRTL_RHI_EXPORT CRTL_ERROR crtl_new_command_buffer(CRTLDevice device,
                                                   CRTLCommandAllocator cmd_allocator,
                                                   CRTLCommandBuffer *cmd_buffer);

CRTL_RHI_EXPORT CRTL_ERROR crtl_close_command_buffer(CRTLDevice device,
                                                     CRTLCommandBuffer cmd_buffer);

/* Submit the command buffer to be run on the queue. If an event is passed,
 * it will be signaled when the command buffer is complete
 */
CRTL_RHI_EXPORT CRTL_ERROR crtl_submit_command_buffer(CRTLDevice device,
                                                      CRTLQueue queue,
                                                      CRTLCommandBuffer cmd_buffer,
                                                      CRTLEvent event);

CRTL_RHI_EXPORT CRTL_ERROR crtl_copy_buffer_to_buffer(CRTLDevice device,
                                                      CRTLCommandBuffer cmd_buffer,
                                                      CRTLBuffer src,
                                                      uint64_t src_offset,
                                                      CRTLBuffer dst,
                                                      uint64_t dst_offset,
                                                      uint64_t size);

CRTL_RHI_EXPORT CRTL_ERROR crtl_copy_buffer_to_texture(CRTLDevice device,
                                                       CRTLCommandBuffer cmd_buffer,
                                                       CRTLBuffer src,
                                                       uint64_t src_offset,
                                                       CRTLTexture dst,
                                                       CRTLBox3D region);

CRTL_RHI_EXPORT CRTL_ERROR crtl_copy_texture_to_buffer(CRTLDevice device,
                                                       CRTLCommandBuffer cmd_buffer,
                                                       CRTLTexture src,
                                                       CRTLBox3D region,
                                                       CRTLBuffer dst,
                                                       uint64_t dst_offset);

/* TODO/Note to include: build and compaction cannot be done in the same command buffer
 * since we need to readback the compact size and allocate room for it
 */
CRTL_RHI_EXPORT CRTL_ERROR crtl_build_blas(CRTLDevice device,
                                           CRTLCommandBuffer cmd_buffer,
                                           CRTLGroup group);

CRTL_RHI_EXPORT CRTL_ERROR crtl_update_blas(CRTLDevice device,
                                            CRTLCommandBuffer cmd_buffer,
                                            CRTLGroup group);

CRTL_RHI_EXPORT CRTL_ERROR crtl_compact_blas(CRTLDevice device,
                                             CRTLCommandBuffer cmd_buffer,
                                             CRTLGroup group);

CRTL_RHI_EXPORT CRTL_ERROR crtl_build_tlas(CRTLDevice device,
                                           CRTLCommandBuffer cmd_buffer,
                                           CRTLScene scene);

CRTL_RHI_EXPORT CRTL_ERROR crtl_update_tlas(CRTLDevice device,
                                            CRTLCommandBuffer cmd_buffer,
                                            CRTLScene scene);

CRTL_RHI_EXPORT CRTL_ERROR crtl_upload_shader_table(CRTLDevice device,
                                                    CRTLCommandBuffer cmd_buffer,
                                                    CRTLRTPipeline pipeline);

CRTL_RHI_EXPORT CRTL_ERROR crtl_set_rtpipeline(CRTLDevice device,
                                               CRTLCommandBuffer cmd_buffer,
                                               CRTLRTPipeline pipeline);

CRTL_RHI_EXPORT CRTL_ERROR
crtl_set_global_parameter_block(CRTLDevice device,
                                CRTLCommandBuffer cmd_buffer,
                                CRTLGlobalParameterBlock parameter_block);

CRTL_RHI_EXPORT CRTL_ERROR crtl_dispatch_rays(CRTLDevice device,
                                              CRTLCommandBuffer cmd_buffer,
                                              uint32_t width,
                                              uint32_t height);

CRTL_RHI_EXPORT CRTL_ERROR crtl_barrier_global(CRTLDevice device,
                                               CRTLCommandBuffer cmd_buffer,
                                               CRTL_BARRIER_STAGE src_stages,
                                               CRTL_BARRIER_STAGE dst_stages,
                                               CRTL_BARRIER_ACCESS src_accesses,
                                               CRTL_BARRIER_ACCESS dst_accesses);

CRTL_RHI_EXPORT CRTL_ERROR crtl_barrier_buffer(CRTLDevice device,
                                               CRTLCommandBuffer cmd_buffer,
                                               CRTL_BARRIER_STAGE src_stages,
                                               CRTL_BARRIER_STAGE dst_stages,
                                               CRTL_BARRIER_ACCESS src_accesses,
                                               CRTL_BARRIER_ACCESS dst_accesses,
                                               CRTLBuffer buffer,
                                               uint64_t offset,
                                               uint64_t size);

CRTL_RHI_EXPORT CRTL_ERROR crtl_barrier_texture(CRTLDevice device,
                                                CRTLCommandBuffer cmd_buffer,
                                                CRTL_BARRIER_STAGE src_stages,
                                                CRTL_BARRIER_STAGE dst_stages,
                                                CRTL_BARRIER_ACCESS src_accesses,
                                                CRTL_BARRIER_ACCESS dst_accesses,
                                                CRTLTexture texture);

#ifdef __cplusplus
}
#endif
