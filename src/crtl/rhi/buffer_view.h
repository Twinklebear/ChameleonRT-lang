#pragma once

#include <memory>
#include "data_type.h"

namespace crtl {

class BufferBase;

/* A BufferView references a subregion of a buffer providing typed access to the memory.
 * The BufferViewBase allows backends to implement the appropriate mapping to the backend API
 * and for passing views to the backend module.
 */
class BufferViewBase {
    std::shared_ptr<BufferBase> buffer;
    size_t offset_bytes = 0;
    size_t count = 0;
    DataType data_type = DataType::UNKNOWN;

public:
    virtual ~BufferViewBase() = default;
};

/* The BufferView template implements the typed view of the Buffer for the app, using the
 * BufferViewBase to access the backend API
 */
template <typename T, MemorySpace M>
class BufferView {
    std::shared_ptr<BufferViewBase> buffer_view;

public:
    // TODO: Doesn't make sense for device only memory
    // T* data();
};
}
