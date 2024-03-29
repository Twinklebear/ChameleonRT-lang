#pragma once

#include <stdint.h>
#include "crtl_buffer.h"
#include "crtl_device.h"
#include "crtl_geometry.h"
#include "crtl_parameter_block.h"
#include "crtl_shader.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct RTPipeline : APIObject {};
}
typedef crtl_rhi::RTPipeline *CRTLRTPipeline;
#else
typedef CRTLAPIObject CRTLRTPipeline;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Alternative to passing the shader_library to get global params would be to
// perform a "link" step in crtl_build_shader_table where we merge globals from different
// shader libraries by matching their variable names? This would allow separate files to
// be passed to make individual shader libraries that are linked together, at least
// "linked" in the global parameter passing sense. Right now this is a bit error prone,
// it's easy to pass raygen record/miss record/scene using different shader libraries than
// this one or each other to the pipeline
// Error checking can be done using the shader library of the entry point though to
// validate that all shaders in the pipeline are from the same shader library
CRTL_EXPORT CRTL_ERROR crtl_new_rtpipeline(CRTLDevice device,
                                           CRTLShaderLibrary shader_library,
                                           uint32_t n_miss_records,
                                           CRTLRTPipeline *pipeline);

// Some limitations/things to check here: the raygen record has to come from the shader
// library
CRTL_EXPORT CRTL_ERROR crtl_set_raygen_record(CRTLDevice device,
                                              CRTLRTPipeline pipeline,
                                              CRTLShaderRecord raygen_record);

// Some limitations/things to check here: the miss record has to come from the shader
// library
CRTL_EXPORT CRTL_ERROR crtl_set_miss_record(CRTLDevice device,
                                            CRTLRTPipeline pipeline,
                                            uint32_t index,
                                            CRTLShaderRecord miss_record);

// Some limitations/things to check here: the scene has to use shaders all from
// the shader library passed
CRTL_EXPORT CRTL_ERROR crtl_set_scene(CRTLDevice device,
                                      CRTLRTPipeline pipeline,
                                      CRTLScene scene);

// TODO: Also support for callable shaders?

// Will note: build SBT is separate step on host, then uploading it happens on the queue
// so apps can control when the actual GPU upload step happens
CRTL_EXPORT CRTL_ERROR crtl_build_shader_table(CRTLDevice device,
                                               CRTLRTPipeline pipeline);

#ifdef __cplusplus
}
#endif
