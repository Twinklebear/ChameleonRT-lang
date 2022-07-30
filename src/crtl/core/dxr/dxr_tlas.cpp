#include "dxr_tlas.h"
#include <algorithm>
#include <numeric>
#include "dxr_utils.h"
#include "util.h"

namespace crtl {
namespace dxr {

using Microsoft::WRL::ComPtr;

#if 0
TopLevelBVH::TopLevelBVH(Buffer instance_buf,
                         const std::vector<Instance> &instances,
                         D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS build_flags)
    : instances(instances), instance_buf(instance_buf), build_flags(build_flags)
{
}

void TopLevelBVH::enqeue_build(ID3D12Device5 *device, ID3D12GraphicsCommandList4 *cmd_list)
{
    // Determine bound of much memory the accel builder may need and allocate it
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bvh_inputs = {0};
    bvh_inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    bvh_inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    bvh_inputs.NumDescs = instances.size();
    bvh_inputs.InstanceDescs = instance_buf->GetGPUVirtualAddress();
    bvh_inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuild_info = {0};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&bvh_inputs, &prebuild_info);

    // The buffer sizes must be aligned to 256 bytes
    prebuild_info.ResultDataMaxSizeInBytes =
        align_to(prebuild_info.ResultDataMaxSizeInBytes,
                 D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
    prebuild_info.ScratchDataSizeInBytes =
        align_to(prebuild_info.ScratchDataSizeInBytes,
                 D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);

    bvh = Buffer::device(device,
                         prebuild_info.ResultDataMaxSizeInBytes,
                         D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
                         D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    scratch = Buffer::device(device,
                             prebuild_info.ScratchDataSizeInBytes,
                             D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                             D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC build_desc = {0};
    build_desc.Inputs = bvh_inputs;
    build_desc.DestAccelerationStructureData = bvh->GetGPUVirtualAddress();
    build_desc.ScratchAccelerationStructureData = scratch->GetGPUVirtualAddress();
    cmd_list->BuildRaytracingAccelerationStructure(&build_desc, 0, nullptr);

    // Insert a barrier to wait for the build to complete
    D3D12_RESOURCE_BARRIER barrier = barrier_uav(bvh);
    cmd_list->ResourceBarrier(1, &barrier);
}

void TopLevelBVH::finalize()
{
    // Release the buffers we don't need anymore
    scratch = Buffer();
}

size_t TopLevelBVH::num_instances() const
{
    return instances.size();
}

ID3D12Resource *TopLevelBVH::operator->()
{
    return get();
}

ID3D12Resource *TopLevelBVH::get()
{
    return bvh.get();
}
#endif
}
}
