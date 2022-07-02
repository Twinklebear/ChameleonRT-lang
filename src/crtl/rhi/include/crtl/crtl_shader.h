#pragma once

#include "crtl_core.h"
#include "crtl_device.h"
#include "crtl_parameter_block.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct ShaderLibrary : APIObject {};
struct ShaderEntryPoint : APIObject {};
struct ShaderRecord : APIObject {};
}
typedef crtl_rhi::ShaderLibrary *CRTLShaderLibrary;
typedef crtl_rhi::ShaderEntryPoint *CRTLShaderEntryPoint;
typedef crtl_rhi::ShaderRecord *CRTLShaderRecord;
#else
typedef CRTLAPIObject CRTLShaderLibrary;
typedef CRTLAPIObject CRTLShaderEntryPoint;
typedef CRTLAPIObject CRTLShaderRecord;
#endif

#ifdef __cplusplus
extern "C" {
#endif

CRTL_RHI_EXPORT CRTLShaderLibrary crtl_new_shader_library(CRTLDevice device,
                                                          const char *library_src);

CRTL_RHI_EXPORT CRTLShaderEntryPoint crtl_get_shader_entry_point(
    CRTLDevice device, CRTLShaderLibrary shader_library, const char *entry_point);

CRTL_RHI_EXPORT CRTLShaderParameterBlock
crtl_new_shader_parameter_block(CRTLDevice device, CRTLShaderEntryPoint entry_point);

CRTL_RHI_EXPORT CRTLShaderRecord crtl_new_shader_record(CRTLDevice device,
                                                        CRTLShaderEntryPoint entry_point);

CRTL_RHI_EXPORT void crtl_set_shader_parameter_block(
    CRTLDevice device,
    CRTLShaderRecord shader_record,
    CRTLShaderParameterBlock parameter_block);

#ifdef __cplusplus
}
#endif
