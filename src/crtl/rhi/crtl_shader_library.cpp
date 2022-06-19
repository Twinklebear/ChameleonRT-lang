#pragma once

#include "crtl/crtl_shader_library.h"

extern "C" CRTL_RHI_EXPORT CRTLShaderLibrary
crtl_create_shader_library(CRTLDevice device, const char *library_src)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT CRTLShaderEntryPoint crtl_get_shader_entry_point(
    CRTLDevice device, CRTLShaderLibrary shader_library, const char *entry_point)
{
    return nullptr;
}
