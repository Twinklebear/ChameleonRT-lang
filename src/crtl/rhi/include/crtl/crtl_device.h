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

#ifdef __cplusplus
extern "C" {
#endif

// Create a device for the desired API. Will load the backend for the device if not already
// loaded
CRTL_RHI_EXPORT CRTLDevice crtl_new_device(const DEVICE_API api);

// Increase the application refcount for the specified object
CRTL_RHI_EXPORT void crtl_retain(CRTLDevice device, CRTLAPIObject object);

/* Decrement the application refcount for the specified object.
 * Objects are released when their app refcount reaches 0 and they are no longer referenced by
 * any internal objects
 */
CRTL_RHI_EXPORT void crtl_release(CRTLDevice device, CRTLAPIObject object);

#ifdef __cplusplus
}
#endif
