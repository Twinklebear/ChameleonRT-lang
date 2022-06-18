#include "backend.h"

namespace crtl {

std::shared_ptr<Backend> load_backend(const BackendAPI backend_api)
{
    // TODO: here dlopen the backend module and create its backend instance
    // to return to the caller. We should cache the backend creation ptr to not re-load it
    // if we've already loaded it (though the app shouldn't re-create the same backend
    // repeatedly..)
    return nullptr;
}
}
