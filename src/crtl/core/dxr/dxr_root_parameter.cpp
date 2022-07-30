#include "dxr_root_parameter.h"
#include "util.h"

namespace crtl {
namespace dxr {

using Microsoft::WRL::ComPtr;
RootParam::RootParam(D3D12_ROOT_PARAMETER param, const std::string &name)
    : param(param), name(name)
{
}

}
}
