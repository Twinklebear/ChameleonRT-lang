#pragma once

#include <memory>

namespace crtl {
/* Base class for all APIObjects
 */
class APIObject {
public:
    virtual ~APIObject() = default;
};

/* The AppRefCount tracks the retain/release count of the app,
 * so that the Device can release the app's reference to the object when it's done with it
 */
struct AppRefCount {
    int ref_count = 0;
    std::shared_ptr<APIObject> object;
};
}
