#include "crtl/crtl_shader.h"
#include "device.h"

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_shader_library(
    CRTLDevice device, const char *library_src, CRTLShaderLibrary *shader_library)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_shader_library(library_src, shader_library);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_get_shader_entry_point(CRTLDevice device,
                            CRTLShaderLibrary shader_library,
                            const char *entry_point_name,
                            CRTLShaderEntryPoint *entry_point)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->get_shader_entry_point(shader_library, entry_point_name, entry_point);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_new_shader_parameter_block(CRTLDevice device,
                                CRTLShaderEntryPoint entry_point,
                                CRTLShaderParameterBlock *parameter_block)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_shader_parameter_block(entry_point, parameter_block);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_shader_record(
    CRTLDevice device, CRTLShaderEntryPoint entry_point, CRTLShaderRecord *shader_record)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_shader_record(entry_point, shader_record);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_set_shader_parameter_block(CRTLDevice device,
                                CRTLShaderRecord shader_record,
                                CRTLShaderParameterBlock parameter_block)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_shader_parameter_block(shader_record, parameter_block);
}
