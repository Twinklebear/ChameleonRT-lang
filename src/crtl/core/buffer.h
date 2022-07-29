#pragma once

#include "api_object.h"
#include "buffer_view.h"
#include "crtl/crtl_enums.h"

namespace crtl {

/* A Buffer is a region of untyped memory allocated on the device or in
 * device-visible host memory.
 */
class Buffer : APIObject {
    CRTL_MEMORY_SPACE memory_space;
    size_t size_bytes;

public:
    virtual ~Buffer() = default;

    std::shared_ptr<BufferView> make_view();
};

/* The BufferM template is a utility for backend implementations to enforce internal
 * compile time requirements about memory spaces
 */
template <CRTL_MEMORY_SPACE M>
class BufferM : public Buffer {
public:
    BufferM() = default;

    template <typename T>
    std::shared_ptr<BufferViewT<T, M>> make_typed_view();
};

template <CRTL_MEMORY_SPACE M>
template <typename T>
std::shared_ptr<BufferViewT<T, M>> BufferM<M>::make_typed_view()
{
    // TODO: validate the buffer memory space
    return nullptr;
}

}
