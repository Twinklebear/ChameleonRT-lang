#include "crtl/crtl.h"
#include "dxr_device.h"

extern "C" crtl::Device *crtl_dxr_create_device()
{
    auto *d = new crtl::dxr::DXRDevice();
    d->app_ref_count = 1;
    return d;
}

extern "C" CRTL_ERROR crtl_dxr_retain(crtl::Device *d, CRTLAPIObject o)
{
    crtl::APIObject *obj = reinterpret_cast<crtl::APIObject *>(o);
    if (!obj) {
        return CRTL_ERROR_OBJECT_NULL;
    }

    // Check if we're retaining/releasing a device
    {
        crtl::dxr::DXRDevice *obj_device = dynamic_cast<crtl::dxr::DXRDevice *>(obj);
        if (obj_device) {
            obj_device->app_ref_count++;
            return CRTL_ERROR_NONE;
        }
    }

    // Regular API object's app ref counts are managed by their device
    crtl::dxr::DXRDevice *dxd = dynamic_cast<crtl::dxr::DXRDevice *>(d);
    return dxd->retain(obj);
}

extern "C" CRTL_ERROR crtl_dxr_release(crtl::Device *d, CRTLAPIObject o)
{
    crtl::APIObject *obj = reinterpret_cast<crtl::APIObject *>(o);
    if (!obj) {
        return CRTL_ERROR_OBJECT_NULL;
    }

    // Check if we're retaining/releasing a device
    {
        crtl::dxr::DXRDevice *obj_device = dynamic_cast<crtl::dxr::DXRDevice *>(obj);
        if (obj_device) {
            obj_device->app_ref_count--;
            if (obj_device->app_ref_count == 0) {
                delete obj_device;
            }
            return CRTL_ERROR_NONE;
        }
    }

    // Regular API object's app ref counts are managed by their device
    crtl::dxr::DXRDevice *dxd = dynamic_cast<crtl::dxr::DXRDevice *>(d);
    return dxd->release(obj);
}
