#pragma once

#include <string>
#include <vector>
#include "dxr_buffer.h"
#include "dxr_geometry.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

// TODO: This will be used by the group, needs to support mix of triangle geometries and
// user geometries
class BottomLevelBVH {
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS build_flags;
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_DESC post_build_info_desc = {
        0};
    Buffer scratch, post_build_info, post_build_info_readback;

    std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geom_descs;

public:
    std::vector<Geometry> geometries;
    Buffer bvh;

    BottomLevelBVH() = default;

    BottomLevelBVH(
        std::vector<Geometry> &geometries,
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS build_flags =
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE |
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION);

    /* After calling build the commands are placed in the command list, with a
     * UAV barrier to wait on the completion of the build before other commands are
     * run, but does not submit the command list.
     */
    void enqeue_build(ID3D12Device5 *device, ID3D12GraphicsCommandList4 *cmd_list);

    /* Enqueue the BVH compaction copy if the BVH was built with compaction enabled.
     * The BVH build must have been enqueued and completed so that the post build info is
     * available
     */
    void enqueue_compaction(ID3D12Device5 *device, ID3D12GraphicsCommandList4 *cmd_list);

    /* Finalize the BVH build structures to release any scratch space.
     * Must call after enqueue compaction if performing compaction, otherwise
     * this can be called after the work from enqueue build has been finished
     */
    void finalize();

    ID3D12Resource *operator->();
    ID3D12Resource *get();
};

}
}
