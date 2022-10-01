#include "dxr_buffer_view.h"
#include "error.h"
#include "util.h"

namespace crtl {
namespace dxr {

BufferView::BufferView(std::shared_ptr<Buffer> buffer,
                       CRTL_DATA_TYPE data_type,
                       size_t offset_bytes,
                       size_t size)
    : buffer(buffer),
      data_type(data_type),
      offset_bytes(offset_bytes),
      size(size),
      mapping(nullptr),
      mapped_mode(CRTL_BUFFER_MAP_MODE_READ)
{
}

void *BufferView::map(CRTL_BUFFER_MAP_MODE map_mode)
{
    if (mapping) {
        throw Error("Attempt to map a BufferView that is already mapped",
                    CRTL_ERROR_BUFFER_VIEW_ALREADY_MAPPED);
    }

    // Note: map mode/buffer usage validation will be handled higher up in the API
    mapped_mode = map_mode;
    D3D12_RANGE read_range;
    if (map_mode == CRTL_BUFFER_MAP_MODE_READ) {
        read_range.Begin = offset_bytes;
        read_range.End = offset_bytes + size_bytes();
    } else {
        read_range.Begin = 0;
        read_range.End = 0;
    }
    CHECK_ERR(buffer->get()->Map(0, &read_range, reinterpret_cast<void **>(&mapping)));

    // DX12 doesn't offset the mapping, so apply the offset now to return the
    // region the app expects
    mapping += offset_bytes;
    return mapping;
}

void BufferView::unmap()
{
    if (!mapping) {
        throw Error("Attempt to unmap a BufferView that is not mapped",
                    CRTL_ERROR_BUFFER_VIEW_NOT_MAPPED);
    }

    D3D12_RANGE written_range;
    if (mapped_mode == CRTL_BUFFER_MAP_MODE_WRITE) {
        written_range.Begin = offset_bytes;
        written_range.End = offset_bytes + size_bytes();
    } else {
        written_range.Begin = 0;
        written_range.End = 0;
    }
    buffer->get()->Unmap(0, &written_range);
    mapping = nullptr;
}

size_t BufferView::size_bytes()
{
    return size * crtl::data_type_size(data_type);
}

D3D12_GPU_VIRTUAL_ADDRESS BufferView::gpu_virtual_address()
{
    return buffer->gpu_virtual_address() + offset_bytes;
}
}
}
