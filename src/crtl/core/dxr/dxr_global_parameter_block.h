#pragma once

#include "crtl_dxr_export.h"
#include "dxr_parameter_block.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT GlobalParameterBlock : public ParameterBlock {
public:
    // TODO

    void set_parameter(const std::string &name,
                       CRTL_DATA_TYPE data_type,
                       void *parameter) override
    {
    }

    void set_parameter(const std::string &name,
                       CRTL_DATA_TYPE data_type,
                       const std::shared_ptr<BufferView> &parameter) override
    {
    }
};
}
}
