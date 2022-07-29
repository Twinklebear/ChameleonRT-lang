#pragma once

#include <memory>
#include "api_object.h"
#include "crtl/crtl_enums.h"

namespace crtl {

class Buffer;

/* A BufferView references a subregion of a buffer providing typed access to the memory.
 */
class BufferView : public APIObject {
    std::shared_ptr<Buffer> buffer;
    size_t offset_bytes = 0;
    size_t count = 0;
    CRTL_DATA_TYPE data_type = CRTL_TYPE_UNKNOWN;

public:
    virtual ~BufferView() = default;
};

/* The BufferViewT is a utility for backend implementations to provide typed access to the
 * view, templated on both the memory space and view type
 */
template <typename T, CRTL_MEMORY_SPACE M>
class BufferViewT : public BufferView {
public:
    // Doesn't make sense for device only memory
    // T* data();
};
}
