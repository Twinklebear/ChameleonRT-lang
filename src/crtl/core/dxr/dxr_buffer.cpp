#include "dxr_buffer.h"

namespace crtl {
namespace dxr {

D3D12_RESOURCE_DESC Buffer::res_desc(size_t nbytes, D3D12_RESOURCE_FLAGS flags)
{
    D3D12_RESOURCE_DESC desc = {0};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = nbytes;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = flags;
    return desc;
}

Buffer Buffer::create(ID3D12Device *device,
                      size_t nbytes,
                      D3D12_RESOURCE_STATES state,
                      D3D12_HEAP_PROPERTIES props,
                      D3D12_RESOURCE_DESC desc)
{
    if (nbytes == 0) {
        throw std::runtime_error("Error: Cannot create a buffer of size 0");
    }
    Buffer b;
    b.buf_size = nbytes;
    b.rheap = props.Type;
    b.rstate = state;
    CHECK_ERR(device->CreateCommittedResource(
        &props, D3D12_HEAP_FLAG_NONE, &desc, state, nullptr, IID_PPV_ARGS(&b.res)));
    return b;
}

Buffer Buffer::upload(ID3D12Device *device,
                      size_t nbytes,
                      D3D12_RESOURCE_STATES state,
                      D3D12_RESOURCE_FLAGS flags)
{
    return create(device, nbytes, state, UPLOAD_HEAP_PROPS, res_desc(nbytes, flags));
}
Buffer Buffer::device(ID3D12Device *device,
                      size_t nbytes,
                      D3D12_RESOURCE_STATES state,
                      D3D12_RESOURCE_FLAGS flags)
{
    return create(device, nbytes, state, DEFAULT_HEAP_PROPS, res_desc(nbytes, flags));
}
Buffer Buffer::readback(ID3D12Device *device,
                        size_t nbytes,
                        D3D12_RESOURCE_STATES state,
                        D3D12_RESOURCE_FLAGS flags)
{
    return create(device, nbytes, state, READBACK_HEAP_PROPS, res_desc(nbytes, flags));
}

void *Buffer::map()
{
    assert(rheap != D3D12_HEAP_TYPE_DEFAULT);
    void *mapping = nullptr;
    D3D12_RANGE range = {0};
    // Explicitly note we want the whole range to silence debug layer warnings
    range.End = buf_size;
    CHECK_ERR(res->Map(0, &range, &mapping));
    return mapping;
}

void *Buffer::map(D3D12_RANGE read)
{
    assert(rheap != D3D12_HEAP_TYPE_DEFAULT);
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

}
}
