#pragma once

#include <iostream>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <glm/glm.hpp>
#include <wrl.h>
#include "crtl_dxr_export.h"

// Utilities for general DX12 ease of use
#define CHECK_ERR(FN)                                                                \
    {                                                                                \
        auto fn_err = FN;                                                            \
        if (FAILED(fn_err)) {                                                        \
            std::cout << #FN << " failed due to " << std::hex << fn_err << std::endl \
                      << std::flush;                                                 \
            throw std::runtime_error(#FN);                                           \
        }                                                                            \
    }

namespace crtl {
namespace dxr {

/* Find the first DXR capable adapter and create a device for it,
 * returns null if no DXR capable adapter is found
 */
CRTL_DXR_EXPORT Microsoft::WRL::ComPtr<ID3D12Device5> create_dxr_device(
    Microsoft::WRL::ComPtr<IDXGIFactory2> &factory);

// Convenience for making resource transition barriers
// TODO: Won't be needed after moving to enhanced barriers
CRTL_DXR_EXPORT D3D12_RESOURCE_BARRIER barrier_transition(ID3D12Resource *res,
                                                          D3D12_RESOURCE_STATES before,
                                                          D3D12_RESOURCE_STATES after);
CRTL_DXR_EXPORT D3D12_RESOURCE_BARRIER
barrier_transition(Microsoft::WRL::ComPtr<ID3D12Resource> &res,
                   D3D12_RESOURCE_STATES before,
                   D3D12_RESOURCE_STATES after);

// Convenience for making UAV transition barriers
CRTL_DXR_EXPORT D3D12_RESOURCE_BARRIER barrier_uav(ID3D12Resource *res);
CRTL_DXR_EXPORT D3D12_RESOURCE_BARRIER
barrier_uav(Microsoft::WRL::ComPtr<ID3D12Resource> &res);

class Resource;

CRTL_DXR_EXPORT D3D12_RESOURCE_BARRIER barrier_transition(Resource &res,
                                                          D3D12_RESOURCE_STATES after);
CRTL_DXR_EXPORT D3D12_RESOURCE_BARRIER barrier_uav(Resource &res);
}
}
