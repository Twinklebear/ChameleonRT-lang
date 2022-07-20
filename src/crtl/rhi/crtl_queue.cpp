#include "crtl/crtl_queue.h"

extern "C" CRTL_RHI_EXPORT CRTLQueue crtl_new_queue(CRTLDevice device)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT CRTLEvent
crtl_submit_command_buffer(CRTLDevice device, CRTLCommandBuffer cmd_buffer)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT CRTLCommandAllocator
crtl_new_command_allocator(CRTLDevice device, CRTLQueue queue)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void crtl_reset_command_allocator(
    CRTLDevice device, CRTLCommandAllocator cmd_allocator)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT CRTLCommandBuffer
crtl_new_command_buffer(CRTLDevice device, CRTLCommandAllocator cmd_allocator)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void crtl_close_command_buffer(CRTLDevice device,
                                                          CRTLCommandBuffer cmd_buffer)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_copy_buffer_to_buffer(CRTLDevice device,
                                                           CRTLCommandBuffer cmd_buffer,
                                                           CRTLBuffer src,
                                                           uint64_t src_offset,
                                                           CRTLBuffer dst,
                                                           uint64_t dst_offset,
                                                           uint64_t size)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_copy_buffer_to_texture(CRTLDevice device,
                                                            CRTLCommandBuffer cmd_buffer,
                                                            CRTLBuffer src,
                                                            uint64_t src_offset,
                                                            CRTLTexture dst,
                                                            CRTLBox3D region)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_copy_texture_to_buffer(CRTLDevice device,
                                                            CRTLCommandBuffer cmd_buffer,
                                                            CRTLTexture src,
                                                            CRTLBox3D region,
                                                            CRTLBuffer dst,
                                                            uint64_t dst_offset)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_build_blas(CRTLDevice device,
                                                CRTLCommandBuffer cmd_buffer,
                                                CRTLGroup group)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_compact_blas(CRTLDevice device,
                                                  CRTLCommandBuffer cmd_buffer,
                                                  CRTLGroup group)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_build_tlas(CRTLDevice device,
                                                CRTLCommandBuffer cmd_buffer,
                                                CRTLScene scene)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_upload_shader_table(CRTLDevice device,
                                                         CRTLCommandBuffer cmd_buffer,
                                                         CRTLRTPipeline pipeline)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_set_rtpipeline(CRTLDevice device,
                                                    CRTLCommandBuffer cmd_buffer,
                                                    CRTLRTPipeline pipeline)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_set_global_parameter_block(
    CRTLDevice device,
    CRTLCommandBuffer cmd_buffer,
    CRTLGlobalParameterBlock parameter_block)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_dispatch_rays(CRTLDevice device,
                                                   CRTLCommandBuffer cmd_buffer,
                                                   uint32_t width,
                                                   uint32_t height)
{
    // TODO
}
