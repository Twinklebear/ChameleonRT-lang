#pragma once

#include <string>
#include "crtl_dxr_export.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT RootParam {
public:
    D3D12_ROOT_PARAMETER param = {0};
    std::string name;
    size_t offset = 0;
    size_t size = 0;

    RootParam() = default;
    RootParam(D3D12_ROOT_PARAMETER param, const std::string &name);
};
}
}
