#include "dxr_buffer.h"
#include "dxr_enums.h"
#include "dxr_utils.h"
#include "error.h"

namespace crtl {
namespace dxr {

Buffer::Buffer(DXRDevice *device,
               CRTL_MEMORY_SPACE memory_space,
               CRTL_BUFFER_USAGE usages,
               size_t sz_bytes)
    : size_bytes(sz_bytes), buffer_usages(usages)
{
    if (size_bytes == 0) {
        throw Error("Buffer size must be > 0", CRTL_ERROR_INVALID_BUFFER_SIZE);
    }

    // Note: MAP_WRITE will require MEMORY_SPACE_UPLOAD at the higher API validation level
    // similarly, MAP_READ will require MEMORY_SPACE_READBACK
    const auto heap_props = memory_space_to_heap_properties(memory_space);
    heap_type = heap_props.Type;

    // TODO: for resource states:
    // this is a list of the states we want to use the resource in so it might contain
    // conflicting states (copy dst and src) so some filtering of the states needs to be
    // done here to pick which one we're going to make the resource in. Upload/readback
    // heaps also enforce requirements on the state the resource has to be created in
    if (heap_type == D3D12_HEAP_TYPE_UPLOAD) {
        res_state = D3D12_RESOURCE_STATE_GENERIC_READ;
    } else if (heap_type == D3D12_HEAP_TYPE_READBACK) {
        res_state = D3D12_RESOURCE_STATE_COPY_DEST;
    } else {
        if (usages & CRTL_BUFFER_USAGE_SHADER_READ_WRITE) {
            res_flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            res_state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        } else if (usages & CRTL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
            res_flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            res_state = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        } else if (usages & CRTL_BUFFER_USAGE_SHADER_READ) {
            res_state = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        } else if (usages & CRTL_BUFFER_USAGE_COPY_DST) {
            res_state = D3D12_RESOURCE_STATE_COPY_DEST;
        } else if (usages & CRTL_BUFFER_USAGE_COPY_DST) {
            res_state = D3D12_RESOURCE_STATE_COPY_SOURCE;
        } else {
            throw Error("Unhandled/supported buffer usage!?", CRTL_ERROR_UNKNOWN);
        }
    }
    const D3D12_RESOURCE_DESC desc = res_desc();
    CHECK_ERR(device->get_d3d12_device()->CreateCommittedResource(&heap_props,
                                                                  D3D12_HEAP_FLAG_NONE,
                                                                  &desc,
                                                                  res_state,
                                                                  nullptr,
                                                                  IID_PPV_ARGS(&res)));
}

size_t Buffer::size() const
{
    return size_bytes;
}

D3D12_GPU_VIRTUAL_ADDRESS Buffer::gpu_virtual_address()
{
    return get()->GetGPUVirtualAddress();
}

D3D12_RESOURCE_DESC Buffer::res_desc()
{
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = size_bytes;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = res_flags;
    return desc;
}
}
}
