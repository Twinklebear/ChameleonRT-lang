#pragma once

#include "crtl_core.h"
#include "crtl_device.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct ShaderLibrary : APIObject {};
struct ShaderEntryPoint : APIObject {};
}
typedef crtl_rhi::ShaderLibrary *CRTLShaderLibrary;
typedef crtl_rhi::ShaderEntryPoint *CRTLShaderEntryPoint;
#else
typedef CRTLAPIObject CRTLShaderLibrary;
typedef CRTLAPIObject CRTLShaderEntryPoint;
#endif

#ifdef __cplusplus
extern "C" {
#endif

CRTL_RHI_EXPORT CRTLShaderLibrary crtl_create_shader_library(CRTLDevice device,
                                                             const char *library_src);

CRTL_RHI_EXPORT CRTLShaderEntryPoint crtl_get_shader_entry_point(
    CRTLDevice device, CRTLShaderLibrary shader_library, const char *entry_point);

#ifdef __cplusplus
}
#endif
