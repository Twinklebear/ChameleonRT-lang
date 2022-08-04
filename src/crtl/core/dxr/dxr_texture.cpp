#include "dxr_texture.h"
#include "dxr_enums.h"
#include "util.h"

namespace crtl {
namespace dxr {
Texture Texture::device(ID3D12Device *device,
                        glm::uvec2 dims,
                        D3D12_RESOURCE_STATES state,
                        DXGI_FORMAT img_format,
                        D3D12_RESOURCE_FLAGS flags)
{
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = dims.x;
    desc.Height = dims.y;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = img_format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = flags;

    Texture t;
    t.tdims = dims;
    t.res_states = state;
    t.heap_type = D3D12_HEAP_TYPE_DEFAULT;
    t.format = img_format;

    const auto heap_props = memory_space_to_heap_properties(CRTL_MEMORY_SPACE_DEVICE);

    CHECK_ERR(device->CreateCommittedResource(
        &heap_props, D3D12_HEAP_FLAG_NONE, &desc, state, nullptr, IID_PPV_ARGS(&t.res)));
    return t;
}

void Texture::readback(ID3D12GraphicsCommandList4 *cmd_list, Buffer &buf)
{
    D3D12_TEXTURE_COPY_LOCATION dst_desc = {0};
    dst_desc.pResource = buf.get();
    dst_desc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dst_desc.PlacedFootprint.Offset = 0;
    dst_desc.PlacedFootprint.Footprint.Format = format;
    dst_desc.PlacedFootprint.Footprint.Width = tdims.x;
    dst_desc.PlacedFootprint.Footprint.Height = tdims.y;
    dst_desc.PlacedFootprint.Footprint.Depth = 1;
    dst_desc.PlacedFootprint.Footprint.RowPitch = linear_row_pitch();

    D3D12_TEXTURE_COPY_LOCATION src_desc = {0};
    src_desc.pResource = get();
    src_desc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    src_desc.SubresourceIndex = 0;

    D3D12_BOX region = {0};
    region.left = 0;
    region.right = tdims.x;
    region.top = 0;
    region.bottom = tdims.y;
    region.front = 0;
    region.back = 1;
    cmd_list->CopyTextureRegion(&dst_desc, 0, 0, 0, &src_desc, &region);
}

void Texture::upload(ID3D12GraphicsCommandList4 *cmd_list, Buffer &buf)
{
    D3D12_TEXTURE_COPY_LOCATION dst_desc = {0};
    dst_desc.pResource = get();
    dst_desc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst_desc.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION src_desc = {0};
    src_desc.pResource = buf.get();
    src_desc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src_desc.PlacedFootprint.Offset = 0;
    src_desc.PlacedFootprint.Footprint.Format = format;
    src_desc.PlacedFootprint.Footprint.Width = tdims.x;
    src_desc.PlacedFootprint.Footprint.Height = tdims.y;
    src_desc.PlacedFootprint.Footprint.Depth = 1;
    src_desc.PlacedFootprint.Footprint.RowPitch = linear_row_pitch();

    D3D12_BOX region = {0};
    region.left = 0;
    region.right = tdims.x;
    region.top = 0;
    region.bottom = tdims.y;
    region.front = 0;
    region.back = 1;
    cmd_list->CopyTextureRegion(&dst_desc, 0, 0, 0, &src_desc, &region);
}

size_t Texture::linear_row_pitch() const
{
    return align_to(tdims.x * pixel_size(), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
}

size_t Texture::pixel_size() const
{
    // Just the common formats I plan to use
    switch (format) {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return 4;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return 16;
    case DXGI_FORMAT_R16_UINT:
        return 2;
    default:
        throw std::runtime_error("Unhandled format in pixel_size!");
        return -1;
    };
}

DXGI_FORMAT Texture::pixel_format() const
{
    return format;
}

glm::uvec2 Texture::dims() const
{
    return tdims;
}

}
}
