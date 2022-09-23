#pragma once

#include "crtl_dxr_export.h"
#include "dxr_parameter_block.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT ShaderRecordParameterBlock : public ParameterBlock {
public:
    // TODO: This will basically just provide a way to set values for the elements in
    // the shader entry point's phmap::flat_hash_map<std::string, ShaderParameterDesc>
    // parameter_info;
    virtual ~ShaderRecordParameterBlock() = default;
};
}
}
