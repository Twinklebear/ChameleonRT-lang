#include "dxr_utils.h"
#include <cassert>
#include <codecvt>
#include <locale>
#include "dxr_resource.h"
#include "util.h"

namespace crtl {
namespace dxr {

using Microsoft::WRL::ComPtr;

const D3D12_HEAP_PROPERTIES UPLOAD_HEAP_PROPS = {
    D3D12_HEAP_TYPE_UPLOAD,
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
    D3D12_MEMORY_POOL_UNKNOWN,
    0,
    0,
};

const D3D12_HEAP_PROPERTIES DEFAULT_HEAP_PROPS = {
    D3D12_HEAP_TYPE_DEFAULT,
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
    D3D12_MEMORY_POOL_UNKNOWN,
    0,
    0,
};

const D3D12_HEAP_PROPERTIES READBACK_HEAP_PROPS = {
    D3D12_HEAP_TYPE_READBACK,
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
    D3D12_MEMORY_POOL_UNKNOWN,
    0,
    0,
};

ComPtr<ID3D12Device5> create_dxr_device(ComPtr<IDXGIFactory2> &factory)
{
    IDXGIAdapter1 *adapter;
    for (uint32_t i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND;
         ++i) {
        ComPtr<ID3D12Device5> device;
        auto err =
            D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device));
        if (FAILED(err)) {
            std::cout << "Failed to make D3D12 device\n";
            throw std::runtime_error("failed to make d3d12 device\n");
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS5 feature_data = {0};
        CHECK_ERR(device->CheckFeatureSupport(
            D3D12_FEATURE_D3D12_OPTIONS5, &feature_data, sizeof(feature_data)));

        if (feature_data.RaytracingTier >= D3D12_RAYTRACING_TIER_1_0) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            const std::string name = conv.to_bytes(desc.Description);
            return device;
        }
    }
    return nullptr;
}

D3D12_RESOURCE_BARRIER barrier_transition(ID3D12Resource *res,
                                          D3D12_RESOURCE_STATES before,
                                          D3D12_RESOURCE_STATES after)
{
    D3D12_RESOURCE_BARRIER b = {0};
    b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    b.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    b.Transition.StateBefore = before;
    b.Transition.StateAfter = after;
    b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    b.Transition.pResource = res;
    return b;
}

D3D12_RESOURCE_BARRIER barrier_transition(Microsoft::WRL::ComPtr<ID3D12Resource> &res,
                                          D3D12_RESOURCE_STATES before,
                                          D3D12_RESOURCE_STATES after)
{
    return barrier_transition(res.Get(), before, after);
}

D3D12_RESOURCE_BARRIER barrier_transition(Resource &res, D3D12_RESOURCE_STATES after)
{
    D3D12_RESOURCE_BARRIER b = barrier_transition(res.get(), res.state(), after);
    res.rstate = after;
    return b;
}

D3D12_RESOURCE_BARRIER barrier_uav(ID3D12Resource *res)
{
    D3D12_RESOURCE_BARRIER b = {0};
    b.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    b.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    b.UAV.pResource = res;
    return b;
}

D3D12_RESOURCE_BARRIER barrier_uav(Microsoft::WRL::ComPtr<ID3D12Resource> &res)
{
    return barrier_uav(res.Get());
}

D3D12_RESOURCE_BARRIER barrier_uav(Resource &res)
{
    return barrier_uav(res.get());
}
}
}
