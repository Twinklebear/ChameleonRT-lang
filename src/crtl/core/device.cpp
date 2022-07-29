#include "device.h"

namespace crtl {

CRTL_ERROR Device::register_error_callback(CRTLErrorCallback ec)
{
    error_callback = ec;
    return CRTL_ERROR_NONE;
}

void Device::report_error(CRTL_ERROR error, const std::string &message)
{
    if (error_callback) {
        error_callback(error, message.c_str());
    }
}
}
