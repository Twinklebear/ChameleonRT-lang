#include "crtl/crtl_texture.h"
#include "device.h"

extern "C" CRTL_RHI_EXPORT CRTLTexture crtl_new_texture(CRTLDevice device,
                                                        CRTL_TEXTURE_TYPE texture_type,
                                                        CRTL_IMAGE_FORMAT format,
                                                        CRTL_IMAGE_USAGE usages,
                                                        uint32_t dimensions[3])
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_texture(texture_type, format, usages, dimensions);
}
