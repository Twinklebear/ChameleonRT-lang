#pragma once

#include <stdexcept>
#include <string>
#include "crtl/crtl_enums.h"
#include "crtl_export.h"

namespace crtl {
class CRTL_EXPORT Error : public std::runtime_error {
    CRTL_ERROR error;

public:
    Error(const std::string &what, CRTL_ERROR error);

    CRTL_ERROR get_error() const;
};
}
