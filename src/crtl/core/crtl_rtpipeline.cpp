#include "crtl/crtl_rtpipeline.h"
#include "device.h"

extern "C" CRTL_EXPORT CRTL_ERROR crtl_new_rtpipeline(CRTLDevice device,
                                                      CRTLShaderLibrary shader_library,
                                                      uint32_t n_miss_records,
                                                      CRTLRTPipeline *pipeline)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_rtpipeline(shader_library, n_miss_records, pipeline);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_set_raygen_record(CRTLDevice device,
                                                         CRTLRTPipeline pipeline,
                                                         CRTLShaderRecord raygen_record)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_raygen_record(pipeline, raygen_record);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_set_miss_record(CRTLDevice device,
                                                       CRTLRTPipeline pipeline,
                                                       uint32_t index,
                                                       CRTLShaderRecord miss_record)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_miss_record(pipeline, index, miss_record);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_set_scene(CRTLDevice device,
                                                 CRTLRTPipeline pipeline,
                                                 CRTLScene scene)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_scene(pipeline, scene);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_build_shader_table(CRTLDevice device,
                                                          CRTLRTPipeline pipeline)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->build_shader_table(pipeline);
}
