#include "crtl/crtl_event.h"
#include "device.h"

extern "C" CRTL_RHI_EXPORT void crtl_await_event(CRTLDevice device, CRTLEvent event)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    d->await_event(event);
}
