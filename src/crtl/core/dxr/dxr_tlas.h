#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "dxr_buffer.h"
#include "dxr_geometry.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

#if 0
// TODO: Will be used by the Scene
class TopLevelBVH {
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS build_flags =
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
    Buffer scratch;

public:
    // TODO: Need instance for dxr
    //std::vector<Instance> instances;
    Buffer instance_buf, bvh;

    TopLevelBVH() = default;

    TopLevelBVH(Buffer instance_buf,
                const std::vector<Instance> &instances,
                D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS build_flags =
                    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE);

    /* After calling build the commands are placed in the command list, with a
     * UAV barrier to wait on the completion of the build before other commands are
     * run, but does not submit the command list.
     */
    void enqeue_build(ID3D12Device5 *device, ID3D12GraphicsCommandList4 *cmd_list);

    // Free the BVH build scratch space
    void finalize();

    size_t num_instances() const;

    ID3D12Resource *operator->();
    ID3D12Resource *get();
};
#endif

}
}
