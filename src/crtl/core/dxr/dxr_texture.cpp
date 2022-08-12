#include "dxr_texture.h"
#include "dxr_enums.h"
#include "util.h"

namespace crtl {
namespace dxr {
Texture::Texture(DXRDevice *device,
                 CRTL_TEXTURE_TYPE texture_type,
                 CRTL_IMAGE_FORMAT crtl_fmt,
                 CRTL_IMAGE_USAGE usages,
                 glm::uvec3 dimensions)
    : tdims(dimensions),
      format(image_format_to_dxgi_format(crtl_fmt)),
      image_usages(usages)
{
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = tdims.x;
    desc.Height = tdims.y;
    desc.DepthOrArraySize = tdims.z;
    desc.MipLevels = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    if (usages & CRTL_IMAGE_USAGE_SHADER_READ_WRITE) {
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        res_state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    } else {
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
        // If we're starting in a write state, then we want to be in that state initially,
        // otherwise we can be in generic read
        if (usages & CRTL_IMAGE_USAGE_COPY_DST) {
            res_state = D3D12_RESOURCE_STATE_COPY_DEST;
        } else {
            res_state = D3D12_RESOURCE_STATE_GENERIC_READ;
        }
    }

    const auto heap_props = memory_space_to_heap_properties(CRTL_MEMORY_SPACE_DEVICE);

    CHECK_ERR(device->get_d3d12_device()->CreateCommittedResource(&heap_props,
                                                                  D3D12_HEAP_FLAG_NONE,
                                                                  &desc,
                                                                  res_state,
                                                                  nullptr,
                                                                  IID_PPV_ARGS(&res)));
}

/*
Note: kept for reference but this will be implemented in the buffer/texture copy
commands

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
*/

size_t Texture::linear_row_pitch() const
{
    return align_to(tdims.x * pixel_size(), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
}

size_t Texture::pixel_size() const
{
    // TODO: need to keep in sync with the CRTL formats
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

glm::uvec3 Texture::dims() const
{
    return tdims;
}

}
}
