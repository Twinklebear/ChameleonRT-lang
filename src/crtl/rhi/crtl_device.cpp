#include "crtl/crtl_device.h"
#include <array>
#include <memory>
#include "backend_plugin.h"
#include "device.h"

std::array<std::unique_ptr<crtl::BackendPlugin>, UNKNOWN> backends;

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_device(const DEVICE_API api,
                                                      CRTLDevice *device)
{
    if (!backends[api]) {
        // TODO: try/catch here, return error if fail to load module
        backends[api] = std::make_unique<crtl::BackendPlugin>(api);
    }
    *device = reinterpret_cast<CRTLDevice>(backends[api]->create_device());
    return CRTL_ERROR_NONE;
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_register_device_error_callback(CRTLDevice device, CRTLErrorCallback error_callback)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->register_error_callback(error_callback);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_retain(CRTLDevice device, CRTLAPIObject object)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return backends[d->device_api()]->retain(d, object);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_release(CRTLDevice device,
                                                   CRTLAPIObject object)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return backends[d->device_api()]->release(d, object);
}
