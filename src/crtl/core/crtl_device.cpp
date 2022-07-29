#include "crtl/crtl_device.h"
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "backend_plugin.h"
#include "device.h"

std::array<std::unique_ptr<crtl::BackendPlugin>, CRTL_DEVICE_API_UNKNOWN> backends;

extern "C" CRTL_EXPORT CRTL_ERROR crtl_new_device(CRTL_DEVICE_API api, CRTLDevice *device)
{
    if (!backends[api]) {
        try {
            backends[api] = std::make_unique<crtl::BackendPlugin>(api);
        } catch (const std::runtime_error &e) {
            std::cerr << "Failed to load backend: " << e.what() << "\n";
            // TODO: could return more specific errors from the backend plugin loader and
            // have more specific enums
            return CRTL_ERROR_BACKEND_LOADING_FAILED;
        } catch (...) {
            return CRTL_ERROR_BACKEND_LOADING_FAILED;
        }
    }
    *device = reinterpret_cast<CRTLDevice>(backends[api]->create_device());
    return CRTL_ERROR_NONE;
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_get_device_api(CRTLDevice device,
                                                      CRTL_DEVICE_API *api)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    *api = d->device_api();
    return CRTL_ERROR_NONE;
}

extern "C" CRTL_EXPORT CRTL_ERROR
crtl_register_device_error_callback(CRTLDevice device, CRTLErrorCallback error_callback)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->register_error_callback(error_callback);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_retain(CRTLDevice device, CRTLAPIObject object)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return backends[d->device_api()]->retain(d, object);
}

extern "C" CRTL_EXPORT CRTL_ERROR crtl_release(CRTLDevice device, CRTLAPIObject object)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return backends[d->device_api()]->release(d, object);
}
