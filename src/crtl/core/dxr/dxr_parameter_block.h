#pragma once

#include "api_object.h"
#include "crtl_dxr_export.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT ParameterBlock : public crtl::APIObject {
public:
    // TODO
    virtual ~ParameterBlock() = default;
};
}
}