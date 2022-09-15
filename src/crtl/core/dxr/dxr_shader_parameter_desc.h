#pragma once

#include "api_object.h"
#include "crtl_dxr_export.h"
#include "dxr_utils.h"
#include "type.h"

namespace crtl {
namespace dxr {

enum class ShaderParameterType {
    INVALID,
    INLINE_CONSTANT,
    SHADER_RESOURCE_VIEW,
    UNORDERED_ACCESS_VIEW,
    // TODO: samplers, tables
};

/* Describes a shader record parameter, which register type it maps too,
 * the slot/space for that register. Inline constants also specify their offset
 * within the inline constant buffer in bytes
 */
struct CRTL_DXR_EXPORT ShaderParameterDesc {
    ShaderParameterType param_type = ShaderParameterType::INVALID;
    std::shared_ptr<ty::Type> type;

    uint32_t slot = -1;
    uint32_t space = -1;
    uint32_t constant_offset_bytes = -1;

    ShaderParameterDesc() = default;

    ShaderParameterDesc(const ShaderParameterType param_type,
                        const std::shared_ptr<ty::Type> &type,
                        uint32_t slot,
                        uint32_t space,
                        uint32_t constant_offset_bytes = -1);
};
}
}
