#include "crtl/crtl_queue.h"
#include "device.h"

extern "C" CRTL_RHI_EXPORT CRTLQueue crtl_new_queue(CRTLDevice device)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_queue();
}

extern "C" CRTL_RHI_EXPORT CRTLCommandAllocator
crtl_new_command_allocator(CRTLDevice device, CRTLQueue queue)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_command_allocator(queue);
}

extern "C" CRTL_RHI_EXPORT void crtl_reset_command_allocator(
    CRTLDevice device, CRTLCommandAllocator cmd_allocator)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->reset_command_allocator(cmd_allocator);
}

extern "C" CRTL_RHI_EXPORT CRTLCommandBuffer
crtl_new_command_buffer(CRTLDevice device, CRTLCommandAllocator cmd_allocator)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_command_buffer(cmd_allocator);
}

extern "C" CRTL_RHI_EXPORT void crtl_close_command_buffer(CRTLDevice device,
                                                          CRTLCommandBuffer cmd_buffer)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->close_command_buffer(cmd_buffer);
}

extern "C" CRTL_RHI_EXPORT CRTLEvent crtl_submit_command_buffer(
    CRTLDevice device, CRTLQueue queue, CRTLCommandBuffer cmd_buffer)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->submit_command_buffer(queue, cmd_buffer);
}

extern "C" CRTL_RHI_EXPORT void crtl_copy_buffer_to_buffer(CRTLDevice device,
                                                           CRTLCommandBuffer cmd_buffer,
                                                           CRTLBuffer src,
                                                           uint64_t src_offset,
                                                           CRTLBuffer dst,
                                                           uint64_t dst_offset,
                                                           uint64_t size)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->copy_buffer_to_buffer(cmd_buffer, src, src_offset, dst, dst_offset, size);
}

extern "C" CRTL_RHI_EXPORT void crtl_copy_buffer_to_texture(CRTLDevice device,
                                                            CRTLCommandBuffer cmd_buffer,
                                                            CRTLBuffer src,
                                                            uint64_t src_offset,
                                                            CRTLTexture dst,
                                                            CRTLBox3D region)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->copy_buffer_to_texture(cmd_buffer, src, src_offset, dst, region);
}

extern "C" CRTL_RHI_EXPORT void crtl_copy_texture_to_buffer(CRTLDevice device,
                                                            CRTLCommandBuffer cmd_buffer,
                                                            CRTLTexture src,
                                                            CRTLBox3D region,
                                                            CRTLBuffer dst,
                                                            uint64_t dst_offset)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->copy_texture_to_buffer(cmd_buffer, src, region, dst, dst_offset);
}

extern "C" CRTL_RHI_EXPORT void crtl_build_blas(CRTLDevice device,
                                                CRTLCommandBuffer cmd_buffer,
                                                CRTLGroup group)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->build_blas(cmd_buffer, group);
}

extern "C" CRTL_RHI_EXPORT void crtl_compact_blas(CRTLDevice device,
                                                  CRTLCommandBuffer cmd_buffer,
                                                  CRTLGroup group)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->compact_blas(cmd_buffer, group);
}

extern "C" CRTL_RHI_EXPORT void crtl_build_tlas(CRTLDevice device,
                                                CRTLCommandBuffer cmd_buffer,
                                                CRTLScene scene)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->build_tlas(cmd_buffer, scene);
}

extern "C" CRTL_RHI_EXPORT void crtl_upload_shader_table(CRTLDevice device,
                                                         CRTLCommandBuffer cmd_buffer,
                                                         CRTLRTPipeline pipeline)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->upload_shader_table(cmd_buffer, pipeline);
}

extern "C" CRTL_RHI_EXPORT void crtl_set_rtpipeline(CRTLDevice device,
                                                    CRTLCommandBuffer cmd_buffer,
                                                    CRTLRTPipeline pipeline)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->set_rtpipeline(cmd_buffer, pipeline);
}

extern "C" CRTL_RHI_EXPORT void crtl_set_global_parameter_block(
    CRTLDevice device,
    CRTLCommandBuffer cmd_buffer,
    CRTLGlobalParameterBlock parameter_block)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->set_global_parameter_block(cmd_buffer, parameter_block);
}

extern "C" CRTL_RHI_EXPORT void crtl_dispatch_rays(CRTLDevice device,
                                                   CRTLCommandBuffer cmd_buffer,
                                                   uint32_t width,
                                                   uint32_t height)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->dispatch_rays(cmd_buffer, width, height);
}
