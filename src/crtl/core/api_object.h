#pragma once

#include "crtl_export.h"

namespace crtl {
class CRTL_EXPORT APIObject {
public:
    int app_ref_count = 0;

    virtual ~APIObject() = default;
};
}
