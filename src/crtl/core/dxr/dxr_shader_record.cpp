#include "dxr_shader_record.h"

namespace crtl {
namespace dxr {

HitGroup::HitGroup(const std::wstring &name,
                   D3D12_HIT_GROUP_TYPE type,
                   const std::wstring &closest_hit,
                   const std::wstring &any_hit,
                   const std::wstring &intersection)
    : name(name),
      closest_hit(closest_hit),
      type(type),
      any_hit(any_hit),
      intersection(intersection)
{
}

bool HitGroup::has_any_hit() const
{
    return !any_hit.empty();
}
bool HitGroup::has_intersection() const
{
    return !intersection.empty();
}

RootSignatureAssociation::RootSignatureAssociation(
    const std::vector<std::wstring> &shaders, const RootSignature &signature)
    : shaders(shaders), signature(signature)
{
}

}
}
