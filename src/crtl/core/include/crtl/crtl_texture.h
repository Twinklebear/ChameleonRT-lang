#pragma once

#include <stdint.h>
#include "crtl_device.h"
#include "crtl_enums.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct Texture : APIObject {};
}
typedef crtl_rhi::Texture *CRTLTexture;
#else
typedef CRTLAPIObject CRTLTexture;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Allocate a new texture with the desired format and dimensionality
 * TODO: Also need to pass usage modes here, for vulkan/dx12
 */
CRTL_EXPORT CRTL_ERROR crtl_new_texture(CRTLDevice device,
                                        CRTL_TEXTURE_TYPE texture_type,
                                        CRTL_IMAGE_FORMAT format,
                                        CRTL_IMAGE_USAGE usages,
                                        const uint32_t dimensions[3],
                                        CRTLTexture *texture);

#ifdef __cplusplus
}
#endif
