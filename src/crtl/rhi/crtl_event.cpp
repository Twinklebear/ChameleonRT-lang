#include "crtl/crtl_event.h"
#include "device.h"

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_event(CRTLDevice device, CRTLEvent *event)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_event(event);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_await_event(CRTLDevice device, CRTLEvent event)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->await_event(event);
}
