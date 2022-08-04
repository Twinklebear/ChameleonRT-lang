#include "dxr_resource.h"

namespace crtl {
namespace dxr {

Resource::~Resource() {}

ID3D12Resource *Resource::operator->()
{
    return get();
}
const ID3D12Resource *Resource::operator->() const
{
    return get();
}
ID3D12Resource *Resource::get()
{
    return res.Get();
}
const ID3D12Resource *Resource::get() const
{
    return res.Get();
}
D3D12_HEAP_TYPE Resource::heap() const
{
    return heap_type;
}
D3D12_RESOURCE_STATES Resource::state() const
{
    return res_states;
}
}
}
