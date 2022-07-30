#pragma once

#include <string>
#include <vector>
#include "dxr_root_signature.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

struct HitGroup {
    std::wstring name, closest_hit, any_hit, intersection;
    D3D12_HIT_GROUP_TYPE type;

    HitGroup() = default;
    HitGroup(const std::wstring &name,
             D3D12_HIT_GROUP_TYPE type,
             const std::wstring &closest_hit,
             const std::wstring &any_hit = L"",
             const std::wstring &intersection = L"");

    bool has_any_hit() const;
    bool has_intersection() const;
};

// TODO: Also miss and raygen records

struct RootSignatureAssociation {
    std::vector<std::wstring> shaders;
    RootSignature signature;

    RootSignatureAssociation() = default;
    RootSignatureAssociation(const std::vector<std::wstring> &shaders,
                             const RootSignature &signature);
};

}
}
