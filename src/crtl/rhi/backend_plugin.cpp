#include "backend_plugin.h"
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#define SHARED_LIBRARY_PREFIX ""
#define SHARED_LIBRARY_SUFFIX ".dll"
#else
#define SHARED_LIBRARY_PREFIX "lib"
#define SHARED_LIBRARY_SUFFIX ".so"
#endif

namespace crtl {

const std::string device_api_to_string(const DEVICE_API api)
{
    switch (api) {
    case DX12:
        return "dx12";
    case VULKAN:
        return "vulkan";
    case METAL:
        return "metal";
    case OPTIX:
        return "optix";
    case EMBREE:
        return "embree";
    default:
        return "unknown";
    }
}

BackendPlugin::BackendPlugin(DEVICE_API api) : api(api)
{
    const std::string backend_name = device_api_to_string(api);
    const std::string backend_file_name = std::string(SHARED_LIBRARY_PREFIX) +
                                          backend_name +
                                          std::string(SHARED_LIBRARY_SUFFIX);
    std::string error_msg;
#ifdef _WIN32
    module_handle = LoadLibrary(backend_file_name.c_str());
    if (!module_handle) {
        auto err = GetLastError();
        LPTSTR msg_buf;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      err,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR)&msg_buf,
                      0,
                      NULL);
        error_msg = msg_buf;
        LocalFree(msg_buf);
    }
#else
    module_handle = dlopen(backend_file_name.c_str(), RTLD_LAZY);
    if (!module_handle) {
        error_msg = dlerror();
    }
#endif

    const std::string base_error_message = "Failed to load backend '" + backend_name +
                                           "' (in " + backend_file_name + ") due to: ";
    if (!module_handle) {
        std::cerr << base_error_message << error_msg << "\n" << std::flush;
        throw std::runtime_error(base_error_message + error_msg);
    }

    const std::string create_device_fn_name = "crtl_" + backend_name + "_create_device";
    create_device_fn = get_fn<CreateDeviceFn>(create_device_fn_name);
    if (!create_device_fn) {
        std::cerr << base_error_message << "Library did not contain function '"
                  << create_device_fn_name << "'\n"
                  << std::flush;
        throw std::runtime_error(base_error_message +
                                 "Library did not contain function '" +
                                 create_device_fn_name + "'");
    }

    const std::string retain_fn_name = "crtl_" + backend_name + "_retain";
    retain_fn = get_fn<RetainFn>(retain_fn_name);
    if (!retain_fn) {
        std::cerr << base_error_message << "Library did not contain function '"
                  << retain_fn_name << "'\n"
                  << std::flush;
        throw std::runtime_error(base_error_message +
                                 "Library did not contain function '" + retain_fn_name +
                                 "'");
    }

    const std::string release_fn_name = "crtl_" + backend_name + "_release";
    release_fn = get_fn<ReleaseFn>(release_fn_name);
    if (!release_fn) {
        std::cerr << base_error_message << "Library did not contain function '"
                  << release_fn_name << "'\n"
                  << std::flush;
        throw std::runtime_error(base_error_message +
                                 "Library did not contain function '" + release_fn_name +
                                 "'");
    }
}

BackendPlugin::~BackendPlugin()
{
#ifdef _WIN32
    FreeLibrary(module_handle);
#else
    dlclose(module_handle);
#endif
}

Device *BackendPlugin::create_device() const
{
    return create_device_fn();
}

void BackendPlugin::retain(Device *d, CRTLAPIObject o) const
{
    retain_fn(d, o);
}

void BackendPlugin::release(Device *d, CRTLAPIObject o) const
{
    release_fn(d, o);
}
}
