#include "error.h"

namespace crtl {

Error::Error(const std::string &what, CRTL_ERROR error)
    : std::runtime_error(what), error(error)
{
}

CRTL_ERROR Error::get_error() const
{
    return error;
}

}
