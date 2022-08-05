#pragma once

#include "crtl_dxr_export.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT Resource {
protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
    D3D12_HEAP_TYPE heap_type;
    // TODO: Won't need to track these resource states with enhanced barriers,
    // but will need to track some usage info.
    D3D12_RESOURCE_STATES res_states;

    friend D3D12_RESOURCE_BARRIER barrier_transition(Resource &res,
                                                     D3D12_RESOURCE_STATES after);

public:
    virtual ~Resource();

    ID3D12Resource *operator->();
    const ID3D12Resource *operator->() const;

    ID3D12Resource *get();
    const ID3D12Resource *get() const;

    D3D12_HEAP_TYPE heap() const;

    D3D12_RESOURCE_STATES state() const;
};
}
}
