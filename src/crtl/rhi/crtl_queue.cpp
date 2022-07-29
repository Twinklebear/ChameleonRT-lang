#include "crtl/crtl_queue.h"
#include "device.h"

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_queue(CRTLDevice device, CRTLQueue *queue)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_queue(queue);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_command_allocator(
    CRTLDevice device, CRTLQueue queue, CRTLCommandAllocator *cmd_allocator)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_command_allocator(queue, cmd_allocator);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_reset_command_allocator(CRTLDevice device, CRTLCommandAllocator cmd_allocator)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->reset_command_allocator(cmd_allocator);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_command_buffer(
    CRTLDevice device, CRTLCommandAllocator cmd_allocator, CRTLCommandBuffer *cmd_buffer)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_command_buffer(cmd_allocator, cmd_buffer);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_close_command_buffer(CRTLDevice device, CRTLCommandBuffer cmd_buffer)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->close_command_buffer(cmd_buffer);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_submit_command_buffer(
    CRTLDevice device, CRTLQueue queue, CRTLCommandBuffer cmd_buffer, CRTLEvent event)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->submit_command_buffer(queue, cmd_buffer, event);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_copy_buffer_to_buffer(CRTLDevice device,
                           CRTLCommandBuffer cmd_buffer,
                           CRTLBuffer src,
                           uint64_t src_offset,
                           CRTLBuffer dst,
                           uint64_t dst_offset,
                           uint64_t size)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->copy_buffer_to_buffer(cmd_buffer, src, src_offset, dst, dst_offset, size);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_copy_buffer_to_texture(CRTLDevice device,
                            CRTLCommandBuffer cmd_buffer,
                            CRTLBuffer src,
                            uint64_t src_offset,
                            CRTLTexture dst,
                            CRTLBox3D region)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->copy_buffer_to_texture(cmd_buffer, src, src_offset, dst, region);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_copy_texture_to_buffer(CRTLDevice device,
                            CRTLCommandBuffer cmd_buffer,
                            CRTLTexture src,
                            CRTLBox3D region,
                            CRTLBuffer dst,
                            uint64_t dst_offset)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->copy_texture_to_buffer(cmd_buffer, src, region, dst, dst_offset);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_build_blas(CRTLDevice device,
                                                      CRTLCommandBuffer cmd_buffer,
                                                      CRTLGroup group)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->build_blas(cmd_buffer, group);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_update_blas(CRTLDevice device,
                                                       CRTLCommandBuffer cmd_buffer,
                                                       CRTLGroup group)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->update_blas(cmd_buffer, group);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_compact_blas(CRTLDevice device,
                                                        CRTLCommandBuffer cmd_buffer,
                                                        CRTLGroup group)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->compact_blas(cmd_buffer, group);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_build_tlas(CRTLDevice device,
                                                      CRTLCommandBuffer cmd_buffer,
                                                      CRTLScene scene)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->build_tlas(cmd_buffer, scene);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_update_tlas(CRTLDevice device,
                                                       CRTLCommandBuffer cmd_buffer,
                                                       CRTLScene scene)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->update_tlas(cmd_buffer, scene);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_upload_shader_table(
    CRTLDevice device, CRTLCommandBuffer cmd_buffer, CRTLRTPipeline pipeline)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->upload_shader_table(cmd_buffer, pipeline);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_set_rtpipeline(CRTLDevice device,
                                                          CRTLCommandBuffer cmd_buffer,
                                                          CRTLRTPipeline pipeline)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_rtpipeline(cmd_buffer, pipeline);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_set_global_parameter_block(CRTLDevice device,
                                CRTLCommandBuffer cmd_buffer,
                                CRTLGlobalParameterBlock parameter_block)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_global_parameter_block(cmd_buffer, parameter_block);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_dispatch_rays(CRTLDevice device,
                                                         CRTLCommandBuffer cmd_buffer,
                                                         uint32_t width,
                                                         uint32_t height)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->dispatch_rays(cmd_buffer, width, height);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_barrier_global(CRTLDevice device,
                    CRTLCommandBuffer cmd_buffer,
                    CRTL_BARRIER_STAGE src_stages,
                    CRTL_BARRIER_STAGE dst_stages,
                    CRTL_BARRIER_ACCESS src_accesses,
                    CRTL_BARRIER_ACCESS dst_accesses)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->barrier_global(
        cmd_buffer, src_stages, dst_stages, src_accesses, dst_accesses);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_barrier_buffer(CRTLDevice device,
                    CRTLCommandBuffer cmd_buffer,
                    CRTL_BARRIER_STAGE src_stages,
                    CRTL_BARRIER_STAGE dst_stages,
                    CRTL_BARRIER_ACCESS src_accesses,
                    CRTL_BARRIER_ACCESS dst_accesses,
                    CRTLBuffer buffer,
                    uint64_t offset,
                    uint64_t size)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->barrier_buffer(cmd_buffer,
                             src_stages,
                             dst_stages,
                             src_accesses,
                             dst_accesses,
                             buffer,
                             offset,
                             size);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_barrier_texture(CRTLDevice device,
                     CRTLCommandBuffer cmd_buffer,
                     CRTL_BARRIER_STAGE src_stages,
                     CRTL_BARRIER_STAGE dst_stages,
                     CRTL_BARRIER_ACCESS src_accesses,
                     CRTL_BARRIER_ACCESS dst_accesses,
                     CRTLTexture texture)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->barrier_texture(
        cmd_buffer, src_stages, dst_stages, src_accesses, dst_accesses, texture);
}
