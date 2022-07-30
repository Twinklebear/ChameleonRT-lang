#include "dxr_descriptor_heap.h"
#include <numeric>

namespace crtl {
namespace dxr {

using Microsoft::WRL::ComPtr;

void DescriptorHeapBuilder::add_range(D3D12_DESCRIPTOR_RANGE_TYPE type,
                                      uint32_t size,
                                      uint32_t base_register,
                                      uint32_t space)
{
    D3D12_DESCRIPTOR_RANGE r = {0};
    r.RangeType = type;
    r.NumDescriptors = size;
    r.BaseShaderRegister = base_register;
    r.RegisterSpace = space;
    r.OffsetInDescriptorsFromTableStart = num_descriptors();
    ranges.push_back(r);
}

bool DescriptorHeapBuilder::contains_range_type(D3D12_DESCRIPTOR_RANGE_TYPE type)
{
    return std::find_if(
               ranges.begin(), ranges.end(), [&](const D3D12_DESCRIPTOR_RANGE &r) {
                   return r.RangeType == type;
               }) != ranges.end();
}

uint32_t DescriptorHeapBuilder::num_descriptors()
{
    return std::accumulate(ranges.begin(),
                           ranges.end(),
                           0,
                           [](const uint32_t &n, const D3D12_DESCRIPTOR_RANGE &r) {
                               return n + r.NumDescriptors;
                           });
}

DescriptorHeapBuilder &DescriptorHeapBuilder::add_srv_range(uint32_t size,
                                                            uint32_t base_register,
                                                            uint32_t space)
{
    add_range(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, size, base_register, space);
    return *this;
}
DescriptorHeapBuilder &DescriptorHeapBuilder::add_uav_range(uint32_t size,
                                                            uint32_t base_register,
                                                            uint32_t space)
{
    add_range(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, size, base_register, space);
    return *this;
}
DescriptorHeapBuilder &DescriptorHeapBuilder::add_cbv_range(uint32_t size,
                                                            uint32_t base_register,
                                                            uint32_t space)
{
    add_range(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, size, base_register, space);
    return *this;
}
DescriptorHeapBuilder &DescriptorHeapBuilder::add_sampler_range(uint32_t size,
                                                                uint32_t base_register,
                                                                uint32_t space)
{
    add_range(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, size, base_register, space);
    return *this;
}

DescriptorHeap DescriptorHeapBuilder::create(ID3D12Device *device)
{
    const bool contains_cbv_srv_uav =
        contains_range_type(D3D12_DESCRIPTOR_RANGE_TYPE_CBV) ||
        contains_range_type(D3D12_DESCRIPTOR_RANGE_TYPE_SRV) ||
        contains_range_type(D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
    if (contains_cbv_srv_uav &&
        contains_range_type(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)) {
        throw std::runtime_error(
            "Error: CBV/SRV/UAV descriptors cannot be in the same heap as samplers");
    }

    ComPtr<ID3D12DescriptorHeap> heap;
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {0};
    heap_desc.NumDescriptors = num_descriptors();
    heap_desc.Type = contains_cbv_srv_uav ? D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
                                          : D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    CHECK_ERR(device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&heap)));
    return DescriptorHeap(heap_desc, ranges, heap);
}

DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_DESC desc,
                               std::vector<D3D12_DESCRIPTOR_RANGE> ranges,
                               Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap)
    : desc(desc), ranges(ranges), heap(heap)
{
}

D3D12_ROOT_PARAMETER DescriptorHeap::root_param() const
{
    D3D12_ROOT_PARAMETER param = {0};
    param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    param.DescriptorTable.NumDescriptorRanges = ranges.size();
    param.DescriptorTable.pDescriptorRanges = ranges.data();
    return param;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::gpu_desc_handle()
{
    return heap->GetGPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::cpu_desc_handle()
{
    return heap->GetCPUDescriptorHandleForHeapStart();
}

ID3D12DescriptorHeap *DescriptorHeap::operator->()
{
    return heap.Get();
}

ID3D12DescriptorHeap *DescriptorHeap::get()
{
    return heap.Get();
}

}
}
