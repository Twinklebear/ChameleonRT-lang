#pragma once

#include "crtl_device.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct Event : APIObject {};
}
typedef crtl_rhi::Event *CRTLEvent;
#else
typedef CRTLAPIObject CRTLEvent;
#endif

#ifdef __cplusplus
extern "C" {
#endif

CRTL_RHI_EXPORT void crtl_await_event(CRTLDevice device, CRTLEvent event);

#ifdef __cplusplus
}
#endif
