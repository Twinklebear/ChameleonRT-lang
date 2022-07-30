#pragma once

#include "api_object.h"
#include "dxr_buffer.h"
#include "dxr_resource.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

// TODO: Generalize to support 3D textures
class Texture : public crtl::APIObject, public Resource {
    glm::uvec2 tdims = glm::uvec2(0);
    DXGI_FORMAT format;

public:
    static Texture device(ID3D12Device *device,
                          // TODO: generalize to support 3D textures
                          glm::uvec2 dims,
                          D3D12_RESOURCE_STATES state,
                          DXGI_FORMAT img_format,
                          D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

    // Read the texture data back into the provided buffer
    // buffer size must be aligned to a row pitch of D3D12_TEXTURE_DATA_PITCH_ALIGNMENT
    void readback(ID3D12GraphicsCommandList4 *cmd_list, Buffer &buf);

    // Upload the buffer into the texture
    // buffer size must be aligned to a row pitch of D3D12_TEXTURE_DATA_PITCH_ALIGNMENT
    void upload(ID3D12GraphicsCommandList4 *cmd_list, Buffer &buf);

    size_t linear_row_pitch() const;

    // Size of one pixel, in bytes
    size_t pixel_size() const;

    DXGI_FORMAT pixel_format() const;

    glm::uvec2 dims() const;
};

}
}
