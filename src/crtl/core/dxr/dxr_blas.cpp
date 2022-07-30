#include "dxr_blas.h"
#include <algorithm>
#include <array>
#include "dxr_utils.h"
#include "util.h"

namespace crtl {
namespace dxr {

using Microsoft::WRL::ComPtr;

BottomLevelBVH::BottomLevelBVH(
    std::vector<Geometry> &geoms,
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS build_flags)
    : build_flags(build_flags), geometries(geoms)
{
    std::transform(geometries.begin(),
                   geometries.end(),
                   std::back_inserter(geom_descs),
                   [](const Geometry &g) { return g.desc; });
}

void BottomLevelBVH::enqeue_build(ID3D12Device5 *device,
                                  ID3D12GraphicsCommandList4 *cmd_list)
{
    post_build_info = Buffer::device(device,
                                     sizeof(uint64_t),
                                     D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                     D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    post_build_info_readback =
        Buffer::readback(device, post_build_info.size(), D3D12_RESOURCE_STATE_COPY_DEST);

    post_build_info_desc.DestBuffer = post_build_info->GetGPUVirtualAddress();
    post_build_info_desc.InfoType =
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE;

    // Determine bound of much memory the accel builder may need and allocate it
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bvh_inputs = {0};
    bvh_inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    bvh_inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    bvh_inputs.NumDescs = geom_descs.size();
    bvh_inputs.pGeometryDescs = geom_descs.data();
    bvh_inputs.Flags = build_flags;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuild_info = {0};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&bvh_inputs, &prebuild_info);

    // The buffer sizes must be aligned to 256 bytes
    prebuild_info.ResultDataMaxSizeInBytes =
        align_to(prebuild_info.ResultDataMaxSizeInBytes,
                 D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
    prebuild_info.ScratchDataSizeInBytes =
        align_to(prebuild_info.ScratchDataSizeInBytes,
                 D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);

#if 0
	std::cout << "TriangleMesh BVH will use at most "
		<< pretty_print_count(prebuild_info.ResultDataMaxSizeInBytes) << "b, and scratch of: "
		<< pretty_print_count(prebuild_info.ScratchDataSizeInBytes) << "b\n";
#endif

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
    cmd_list->BuildRaytracingAccelerationStructure(&build_desc, 1, &post_build_info_desc);

    // Insert a barrier to wait for the build to complete, and transition the post build
    // info write buffer to copy source so we can read it back
    std::array<D3D12_RESOURCE_BARRIER, 2> barriers = {
        barrier_uav(bvh),
        barrier_transition(post_build_info, D3D12_RESOURCE_STATE_COPY_SOURCE)};
    cmd_list->ResourceBarrier(barriers.size(), barriers.data());

    // Enqueue a copy of the post-build info to CPU visible memory
    cmd_list->CopyResource(post_build_info_readback.get(), post_build_info.get());
}

void BottomLevelBVH::enqueue_compaction(ID3D12Device5 *device,
                                        ID3D12GraphicsCommandList4 *cmd_list)
{
    uint64_t *map = static_cast<uint64_t *>(post_build_info_readback.map());
    const uint64_t compacted_size =
        align_to(*map, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
#if 0
	std::cout << "Bottom level AS compacted size will be: " << pretty_print_count(compacted_size) << "b\n";
#endif

    post_build_info_readback.unmap();

    if (build_flags &
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION) {
        scratch = Buffer::device(device,
                                 compacted_size,
                                 D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

        cmd_list->CopyRaytracingAccelerationStructure(
            scratch->GetGPUVirtualAddress(),
            bvh->GetGPUVirtualAddress(),
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_COMPACT);

        D3D12_RESOURCE_BARRIER barrier = barrier_uav(scratch);
        cmd_list->ResourceBarrier(1, &barrier);
    }
}

void BottomLevelBVH::finalize()
{
    if (build_flags &
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION) {
        bvh = scratch;
    }
    // Release the buffers we don't need anymore
    scratch = Buffer();
    post_build_info = Buffer();
    post_build_info_readback = Buffer();
}

ID3D12Resource *BottomLevelBVH::operator->()
{
    return get();
}
ID3D12Resource *BottomLevelBVH::get()
{
    return bvh.get();
}

}
}
