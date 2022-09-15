#pragma once

#include "dxr_shader_parameter_desc.h"

namespace crtl {
namespace dxr {

ShaderParameterDesc::ShaderParameterDesc(const ShaderParameterType param_type,
                                         const std::shared_ptr<ty::Type> &type,
                                         uint32_t slot,
                                         uint32_t space,
                                         uint32_t constant_offset_bytes)
    : param_type(param_type),
      type(type),
      slot(slot),
      space(space),
      constant_offset_bytes(constant_offset_bytes)
{
}
}
}
