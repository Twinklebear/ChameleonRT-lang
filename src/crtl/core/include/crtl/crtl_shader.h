#pragma once

#include "crtl_core.h"
#include "crtl_device.h"
#include "crtl_parameter_block.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct ShaderLibrary : APIObject {};
struct ShaderEntryPoint : APIObject {};
struct ShaderRecord : APIObject {};
struct HitGroupRecord : ShaderRecord {};
struct MissRecord : ShaderRecord {};
struct RaygenRecord : ShaderRecord {};
}
typedef crtl_rhi::ShaderLibrary *CRTLShaderLibrary;
typedef crtl_rhi::ShaderEntryPoint *CRTLShaderEntryPoint;
typedef crtl_rhi::ShaderRecord *CRTLShaderRecord;
typedef crtl_rhi::HitGroupRecord *CRTLHitGroupRecord;
typedef crtl_rhi::MissRecord *CRTLMissRecord;
typedef crtl_rhi::RaygenRecord *CRTLRaygenRecord;
#else
typedef CRTLAPIObject CRTLShaderLibrary;
typedef CRTLAPIObject CRTLShaderEntryPoint;
typedef CRTLAPIObject CRTLShaderRecord;
typedef CRTLShaderRecord CRTLHitGroupRecord;
typedef CRTLShaderRecord CRTLMissRecord;
typedef CRTLShaderRecord CRTLRaygenRecord;
#endif

#ifdef __cplusplus
extern "C" {
#endif

CRTL_EXPORT CRTL_ERROR crtl_new_shader_library(CRTLDevice device,
                                               const char *library_src,
                                               CRTLShaderLibrary *shader_library);

CRTL_EXPORT CRTL_ERROR
crtl_new_global_parameter_block(CRTLDevice device,
                                CRTLShaderLibrary shader_library,
                                CRTLGlobalParameterBlock *parameter_block);

CRTL_EXPORT CRTL_ERROR crtl_get_shader_entry_point(CRTLDevice device,
                                                   CRTLShaderLibrary shader_library,
                                                   const char *entry_point_name,
                                                   CRTLShaderEntryPoint *entry_point);

CRTL_EXPORT CRTL_ERROR
crtl_new_hitgroup_record(CRTLDevice device,
                         CRTLShaderEntryPoint closest_hit,
                         CRTLShaderEntryPoint intersection_optional,
                         CRTLShaderEntryPoint any_hit_optional,
                         CRTLHitGroupRecord *shader_record);

CRTL_EXPORT CRTL_ERROR crtl_new_miss_record(CRTLDevice device,
                                            CRTLShaderEntryPoint miss,
                                            CRTLMissRecord *shader_record);

CRTL_EXPORT CRTL_ERROR crtl_new_raygen_record(CRTLDevice device,
                                              CRTLShaderEntryPoint raygen,
                                              CRTLRaygenRecord *shader_record);

CRTL_EXPORT CRTL_ERROR
crtl_new_shader_record_parameter_block(CRTLDevice device,
                                       CRTLShaderRecord shader_record,
                                       CRTLShaderRecordParameterBlock *parameter_block);

CRTL_EXPORT CRTL_ERROR
crtl_set_shader_record_parameter_block(CRTLDevice device,
                                       CRTLShaderRecord shader_record,
                                       CRTLShaderRecordParameterBlock parameter_block);

#ifdef __cplusplus
}
#endif
