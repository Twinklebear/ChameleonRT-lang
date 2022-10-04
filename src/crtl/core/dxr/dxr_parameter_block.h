#pragma once

#include "api_object.h"
#include "crtl/crtl_enums.h"
#include "crtl_dxr_export.h"
#include "dxr_buffer_view.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT ParameterBlock : public crtl::APIObject {
public:
    virtual ~ParameterBlock() = default;

    virtual void set_parameter(const std::string &name,
                               CRTL_DATA_TYPE data_type,
                               void *parameter) = 0;

    virtual void set_parameter(const std::string &name,
                               CRTL_DATA_TYPE data_type,
                               const std::shared_ptr<BufferView> &parameter) = 0;
};
}
}
