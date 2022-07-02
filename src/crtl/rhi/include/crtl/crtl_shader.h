#pragma once

#include "crtl_core.h"
#include "crtl_device.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct ShaderLibrary : APIObject {};
struct ShaderEntryPoint : APIObject {};
struct ShaderRecord : APIObject {};
struct ShaderParameterBlock : APIObject {};
}
typedef crtl_rhi::ShaderLibrary *CRTLShaderLibrary;
typedef crtl_rhi::ShaderEntryPoint *CRTLShaderEntryPoint;
typedef crtl_rhi::ShaderRecord *CRTLShaderRecord;
typedef crtl_rhi::ShaderParameterBlock *CRTLShaderParameterBlock;
#else
typedef CRTLAPIObject CRTLShaderLibrary;
typedef CRTLAPIObject CRTLShaderEntryPoint;
typedef CRTLAPIObject CRTLShaderRecord;
typedef CRTLAPIObject CRTLShaderParameterBlock;
#endif

#ifdef __cplusplus
extern "C" {
#endif

CRTL_RHI_EXPORT CRTLShaderLibrary crtl_new_shader_library(CRTLDevice device,
                                                          const char *library_src);

CRTL_RHI_EXPORT CRTLShaderEntryPoint crtl_get_shader_entry_point(
    CRTLDevice device, CRTLShaderLibrary shader_library, const char *entry_point);

CRTL_RHI_EXPORT CRTLShaderParameterBlock
crtl_new_parameter_block(CRTLDevice device, CRTLShaderEntryPoint entry_point);

CRTL_RHI_EXPORT void crtl_set_parameter(CRTLDevice device,
                                        CRTLShaderParameterBlock parameter_block,
                                        const char *name,
                                        CRTL_DATA_TYPE data_type,
                                        void *parameter);

CRTL_RHI_EXPORT CRTLShaderRecord crtl_new_shader_record(CRTLDevice device,
                                                        CRTLShaderEntryPoint entry_point);

CRTL_RHI_EXPORT void crtl_set_shader_parameter_block(
    CRTLDevice device,
    CRTLShaderRecord shader_record,
    CRTLShaderParameterBlock parameter_block);

#ifdef __cplusplus
}
#endif
