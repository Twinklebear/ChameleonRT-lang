#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <string>
#include "api_object.h"
#include "crtl/crtl_enums.h"
#include "device.h"

namespace crtl {
class BackendPlugin {
    using CreateDeviceFn = Device *(*)();
    using RetainFn = void (*)(Device *, APIObject *);
    using ReleaseFn = void (*)(Device *, APIObject *);

    DEVICE_API api = UNKNOWN;

#ifdef _WIN32
    HMODULE module_handle = 0;
#else
    void *module_handle = nullptr;
#endif

    CreateDeviceFn create_device_fn = nullptr;
    RetainFn retain_fn = nullptr;
    ReleaseFn release_fn = nullptr;

public:
    BackendPlugin(DEVICE_API api);

    ~BackendPlugin();

    Device *create_device() const;

    void retain(Device *d, APIObject *o) const;

    void release(Device *d, APIObject *o) const;

private:
    /* Load a function pointer from the shared library. Returns nullptr if the function was not
     * found
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