#include "crtl/crtl_shader.h"
#include "device.h"

extern "C" CRTL_RHI_EXPORT CRTLShaderLibrary
crtl_new_shader_library(CRTLDevice device, const char *library_src)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_shader_library(library_src);
}

extern "C" CRTL_RHI_EXPORT CRTLShaderEntryPoint crtl_get_shader_entry_point(
    CRTLDevice device, CRTLShaderLibrary shader_library, const char *entry_point)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->get_shader_entry_point(shader_library, entry_point);
}

extern "C" CRTL_RHI_EXPORT CRTLShaderParameterBlock
crtl_new_shader_parameter_block(CRTLDevice device, CRTLShaderEntryPoint entry_point)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_shader_parameter_block(entry_point);
}

extern "C" CRTL_RHI_EXPORT CRTLShaderRecord
crtl_new_shader_record(CRTLDevice device, CRTLShaderEntryPoint entry_point)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_shader_record(entry_point);
}

extern "C" CRTL_RHI_EXPORT void crtl_set_shader_parameter_block(
    CRTLDevice device,
    CRTLShaderRecord shader_record,
    CRTLShaderParameterBlock parameter_block)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->set_shader_parameter_block(shader_record, parameter_block);
}
