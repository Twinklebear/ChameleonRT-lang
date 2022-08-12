#pragma once

#include "api_object.h"
#include "crtl_dxr_export.h"
#include "dxr_buffer.h"
#include "dxr_resource.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT Texture : public crtl::APIObject, public Resource {
    glm::uvec3 tdims = glm::uvec3(0);
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    // The supported usages for this image
    CRTL_IMAGE_USAGE image_usages;

public:
    Texture(DXRDevice *device,
            CRTL_TEXTURE_TYPE texture_type,
            CRTL_IMAGE_FORMAT format,
            CRTL_IMAGE_USAGE usages,
            glm::uvec3 dimensions);

    size_t linear_row_pitch() const;

    // Size of one pixel, in bytes
    size_t pixel_size() const;

    DXGI_FORMAT pixel_format() const;

    glm::uvec3 dims() const;
};
}
}
