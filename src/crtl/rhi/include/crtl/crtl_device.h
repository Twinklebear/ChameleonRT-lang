#pragma once

#include "crtl_core.h"
#include "crtl_enums.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct Device : APIObject {};
}
typedef crtl_rhi::Device *CRTLDevice;
#else
typedef CRTLAPIObject CRTLDevice;
#endif

typedef void (*CRTLErrorCallback)(CRTL_ERROR error, const char *message);

#ifdef __cplusplus
extern "C" {
#endif

/* Create a device for the desired API. Will load the backend for the device if not
 * already loaded
 */
CRTL_RHI_EXPORT CRTL_ERROR crtl_new_device(const CRTL_DEVICE_API api, CRTLDevice *device);

/* Register a callback to be called each time an error occurs to receive a more detailed
 * error message from the device
 */
CRTL_RHI_EXPORT CRTL_ERROR
crtl_register_device_error_callback(CRTLDevice device, CRTLErrorCallback error_callback);

// Increase the application refcount for the specified object
CRTL_RHI_EXPORT CRTL_ERROR crtl_retain(CRTLDevice device, CRTLAPIObject object);

/* Decrement the application refcount for the specified object.
 * Objects are released when their app refcount reaches 0 and they are no longer
 * referenced by any internal objects
 */
CRTL_RHI_EXPORT CRTL_ERROR crtl_release(CRTLDevice device, CRTLAPIObject object);

#ifdef __cplusplus
}
#endif
