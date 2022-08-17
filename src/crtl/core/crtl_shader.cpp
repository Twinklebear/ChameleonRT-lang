#include "crtl/crtl_shader.h"
#include "device.h"

extern "C" CRTL_EXPORT CRTL_ERROR crtl_new_shader_library(
    CRTLDevice device, const char *library_src, CRTLShaderLibrary *shader_library)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_shader_library(library_src, shader_library);
}

extern "C" CRTL_EXPORT CRTL_ERROR
crtl_new_global_parameter_block(CRTLDevice device,
                                CRTLShaderLibrary shader_library,
                                CRTLGlobalParameterBlock *parameter_block)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_global_parameter_block(shader_library, parameter_block);
}

extern "C" CRTL_EXPORT CRTL_ERROR
crtl_get_shader_entry_point(CRTLDevice device,
                            CRTLShaderLibrary shader_library,
                            const char *entry_point_name,
                            CRTLShaderEntryPoint *entry_point)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->get_shader_entry_point(shader_library, entry_point_name, entry_point);
}

extern "C" CRTL_EXPORT CRTL_ERROR
crtl_new_hitgroup_record(CRTLDevice device,
                         CRTLShaderEntryPoint closest_hit,
                         CRTLShaderEntryPoint intersection_optional,
                         CRTLShaderEntryPoint any_hit_optional,
                         CRTLHitGroupRecord *shader_record)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_hitgroup_record(
        closest_hit, intersection_optional, any_hit_optional, shader_record);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_new_miss_record(CRTLDevice device,
                                                       CRTLShaderEntryPoint miss,
                                                       CRTLMissRecord *shader_record)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_miss_record(miss, shader_record);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_new_raygen_record(CRTLDevice device,
                                                         CRTLShaderEntryPoint raygen,
                                                         CRTLRaygenRecord *shader_record)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_raygen_record(raygen, shader_record);
}

extern "C" CRTL_EXPORT CRTL_ERROR
crtl_new_shader_record_parameter_block(CRTLDevice device,
                                       CRTLShaderRecord shader_record,
                                       CRTLShaderRecordParameterBlock *parameter_block)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_shader_record_parameter_block(shader_record, parameter_block);
}

extern "C" CRTL_EXPORT CRTL_ERROR
crtl_set_shader_record_parameter_block(CRTLDevice device,
                                       CRTLShaderRecord shader_record,
                                       CRTLShaderRecordParameterBlock parameter_block)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_shader_record_parameter_block(shader_record, parameter_block);
}
