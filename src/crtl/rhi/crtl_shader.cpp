#include "crtl/crtl_shader.h"

extern "C" CRTL_RHI_EXPORT CRTLShaderLibrary
crtl_new_shader_library(CRTLDevice device, const char *library_src)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT CRTLShaderEntryPoint crtl_get_shader_entry_point(
    CRTLDevice device, CRTLShaderLibrary shader_library, const char *entry_point)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT CRTLShaderParameterBlock
crtl_new_parameter_block(CRTLDevice device, CRTLShaderEntryPoint entry_point)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT CRTLShaderRecord
crtl_new_shader_record(CRTLDevice device, CRTLShaderEntryPoint entry_point)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void crtl_set_shader_parameter_block(
    CRTLDevice device,
    CRTLShaderRecord shader_record,
    CRTLShaderParameterBlock parameter_block)
{
}
