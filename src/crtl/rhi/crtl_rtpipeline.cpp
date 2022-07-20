#include "crtl/crtl_rtpipeline.h"

extern "C" CRTL_RHI_EXPORT CRTLRTPipeline crtl_new_rtpipeline(
    CRTLDevice device, CRTLShaderLibrary shader_library, uint32_t n_miss_records)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void crtl_set_raygen_record(CRTLDevice device,
                                                       CRTLRTPipeline pipeline,
                                                       CRTLShaderRecord raygen_record)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_set_miss_record(CRTLDevice device,
                                                     CRTLRTPipeline pipeline,
                                                     uint32_t index,
                                                     CRTLShaderRecord miss_record)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_set_scene(CRTLDevice device,
                                               CRTLRTPipeline pipeline,
                                               CRTLScene scene)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT void crtl_build_shader_table(CRTLDevice device,
                                                        CRTLRTPipeline pipeline)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT CRTLGlobalParameterBlock
crtl_new_global_parameter_block(CRTLDevice device, CRTLRTPipeline pipeline)
{
    return nullptr;
}
