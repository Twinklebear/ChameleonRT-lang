#pragma once

#include "crtl/crtl_device.h"
#include <array>
#include <memory>
#include "api_object.h"
#include "backend_plugin.h"
#include "device.h"

std::array<std::unique_ptr<crtl::BackendPlugin>, UNKNOWN> backends;

extern "C" CRTL_RHI_EXPORT CRTLDevice crtl_create_device(const DEVICE_API api)
{
    if (!backends[api]) {
        backends[api] = std::make_unique<crtl::BackendPlugin>(api);
    }
    return reinterpret_cast<CRTLDevice>(backends[api]->create_device());
}

extern "C" CRTL_RHI_EXPORT void crtl_retain(CRTLDevice device, CRTLAPIObject object)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    crtl::APIObject *o = reinterpret_cast<crtl::APIObject *>(object);
    backends[d->device_api()]->retain(d, o);
}

extern "C" CRTL_RHI_EXPORT void crtl_release(CRTLDevice device, CRTLAPIObject object)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    crtl::APIObject *o = reinterpret_cast<crtl::APIObject *>(object);
    backends[d->device_api()]->release(d, o);
}
