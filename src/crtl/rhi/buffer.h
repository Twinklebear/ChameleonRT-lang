#pragma once

#include "buffer_view.h"
#include "data_type.h"

namespace crtl {

/* The BufferBase allows backends to implement the appropriate mapping
 * for the Buffer and for buffers to be passed to the backend's module
 * without pulling in the backend code.
 */
class BufferBase {
    MemorySpace memory_space;
    size_t size_bytes;

public:
    virtual ~BufferBase() = default;
};

/* A Buffer is a region of untyped memory allocated on the device or in
 * device-visible host memory. The Buffer<M> implements the Buffer templated over the memory
 * space to enforce requirements in the API
 */
template <MemorySpace M>
class Buffer {
    std::shared_ptr<BufferBase> buffer;

public:
    template <typename T>
    std::shared_ptr<BufferView<T, M>> make_view();
};

template <MemorySpace M>
template <typename T>
std::shared_ptr<BufferView<T, M>> Buffer<M>::make_view()
{
    return nullptr;
}

}
