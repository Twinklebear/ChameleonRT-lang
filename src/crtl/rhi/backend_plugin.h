#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <string>
#include "crtl/crtl.h"
#include "device.h"

namespace crtl {
class BackendPlugin {
    using CreateDeviceFn = Device *(*)();
    using RetainFn = CRTL_ERROR (*)(Device *, CRTLAPIObject);
    using ReleaseFn = CRTL_ERROR (*)(Device *, CRTLAPIObject);

    CRTL_DEVICE_API api = CRTL_DEVICE_API_UNKNOWN;

#ifdef _WIN32
    HMODULE module_handle = 0;
#else
    void *module_handle = nullptr;
#endif

    CreateDeviceFn create_device_fn = nullptr;
    RetainFn retain_fn = nullptr;
    ReleaseFn release_fn = nullptr;

public:
    BackendPlugin(CRTL_DEVICE_API api);

    ~BackendPlugin();

    Device *create_device() const;

    // Retain/release not managed by the device so that we can also retain/release the
    // device
    CRTL_ERROR retain(Device *d, CRTLAPIObject o) const;

    CRTL_ERROR release(Device *d, CRTLAPIObject o) const;

private:
    /* Load a function pointer from the shared library. Returns nullptr if the function
     * was not found
     */
    template <typename T>
    T get_fn(const std::string &fcn_name)
    {
#ifdef _WIN32
        FARPROC fn = GetProcAddress(module_handle, fcn_name.c_str());
#else
        void *fn = dlsym(module_handle, fcn_name.c_str());
#endif
        return fn ? reinterpret_cast<T>(fn) : nullptr;
    }
};
}
