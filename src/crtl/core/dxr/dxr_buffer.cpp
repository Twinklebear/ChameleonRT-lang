#include "dxr_buffer.h"
#include "dxr_enums.h"
#include "dxr_utils.h"
#include "error.h"

namespace crtl {
namespace dxr {

Buffer::Buffer(DXRDevice *device,
               CRTL_MEMORY_SPACE memory_space,
               CRTL_BUFFER_USAGE usages,
               size_t bytes)
    : buf_size(bytes)
{
    if (buf_size == 0) {
        throw Error("Buffer size must be > 0", CRTL_ERROR_INVALID_BUFFER_SIZE);
    }

    const auto heap_props = memory_space_to_heap_properties(memory_space);
    heap_type = heap_props.Type;

    // TODO: for resource states:
    // this is a list of the states we want to use the resource in so it might contain
    // conflicting states (copy dst and src) so some filtering of the states needs to be
    // done here to pick which one we're going to make the resource in. Upload/readback
    // heaps also enforce requirements on the state the resource has to be created in
    if (heap_type == D3D12_HEAP_TYPE_UPLOAD) {
        res_states = D3D12_RESOURCE_STATE_GENERIC_READ;
    } else if (heap_type == D3D12_HEAP_TYPE_READBACK) {
        res_states = D3D12_RESOURCE_STATE_COPY_DEST;
    } else {
        if (usages & CRTL_BUFFER_USAGE_SHADER_READ_WRITE) {
            res_flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            res_states = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        } else if (usages & CRTL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
            res_flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            res_states = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        } else if (usages & CRTL_BUFFER_USAGE_SHADER_READ) {
            res_states = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        } else if (usages & CRTL_BUFFER_USAGE_COPY_DST) {
            res_states = D3D12_RESOURCE_STATE_COPY_DEST;
        } else if (usages & CRTL_BUFFER_USAGE_COPY_DST) {
            res_states = D3D12_RESOURCE_STATE_COPY_SOURCE;
        } else {
            throw Error("Unhandled/supported buffer usage!?", CRTL_ERROR_UNKNOWN);
        }
    }
    const D3D12_RESOURCE_DESC desc = res_desc();
    CHECK_ERR(device->d3d12_device()->CreateCommittedResource(&heap_props,
                                                              D3D12_HEAP_FLAG_NONE,
                                                              &desc,
                                                              res_states,
                                                              nullptr,
                                                              IID_PPV_ARGS(&res)));
}

void *Buffer::map()
{
    assert(heap_type != D3D12_HEAP_TYPE_DEFAULT);
    void *mapping = nullptr;
    D3D12_RANGE range = {0};
    // Explicitly note we want the whole range to silence debug layer warnings
    range.End = buf_size;
    CHECK_ERR(res->Map(0, &range, &mapping));
    return mapping;
}

void *Buffer::map(D3D12_RANGE read)
{
    assert(heap_type != D3D12_HEAP_TYPE_DEFAULT);
    void *mapping = nullptr;
    CHECK_ERR(res->Map(0, &read, &mapping));
    return mapping;
}

void Buffer::unmap()
{
    res->Unmap(0, nullptr);
}

void Buffer::unmap(D3D12_RANGE written)
{
    res->Unmap(0, &written);
}

size_t Buffer::size() const
{
    return buf_size;
}

D3D12_RESOURCE_DESC Buffer::res_desc()
{
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = buf_size;
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
