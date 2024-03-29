#include "crtl/crtl_texture.h"
#include "device.h"

extern "C" CRTL_EXPORT CRTL_ERROR crtl_new_texture(CRTLDevice device,
                                                   CRTL_TEXTURE_TYPE texture_type,
                                                   CRTL_IMAGE_FORMAT format,
                                                   CRTL_IMAGE_USAGE usages,
                                                   const uint32_t dimensions[3],
                                                   CRTLTexture *texture)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_texture(texture_type, format, usages, dimensions, texture);
}
