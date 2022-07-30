#pragma once

#include <string>
#include <vector>
#include "dxr_root_parameter.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class DescriptorHeap;

class DescriptorHeapBuilder {
    std::vector<D3D12_DESCRIPTOR_RANGE> ranges;

    void add_range(D3D12_DESCRIPTOR_RANGE_TYPE type,
                   uint32_t size,
                   uint32_t base_register,
                   uint32_t space);

    bool contains_range_type(D3D12_DESCRIPTOR_RANGE_TYPE type);
    uint32_t num_descriptors();

public:
    // TODO: We can autocompute the table offsets based on the order that add_* is called
    // if we use the root sig to also guide the desc. heap write
    // TODO: Each range can be assigned a name
    DescriptorHeapBuilder &add_srv_range(uint32_t size,
                                         uint32_t base_register,
                                         uint32_t space);
    DescriptorHeapBuilder &add_uav_range(uint32_t size,
                                         uint32_t base_register,
                                         uint32_t space);
    DescriptorHeapBuilder &add_cbv_range(uint32_t size,
                                         uint32_t base_register,
                                         uint32_t space);
    DescriptorHeapBuilder &add_sampler_range(uint32_t size,
                                             uint32_t base_register,
                                             uint32_t space);

    DescriptorHeap create(ID3D12Device *device);
};

class DescriptorHeap : public RootParam {
    D3D12_DESCRIPTOR_HEAP_DESC desc = {0};
    std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap = nullptr;

    friend class DescriptorHeapBuilder;

    DescriptorHeap(D3D12_DESCRIPTOR_HEAP_DESC desc,
                   std::vector<D3D12_DESCRIPTOR_RANGE> ranges,
                   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap);

public:
    DescriptorHeap() = default;

    D3D12_ROOT_PARAMETER root_param() const;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_desc_handle();
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc_handle();

    ID3D12DescriptorHeap *operator->();
    ID3D12DescriptorHeap *get();
};

}
}
