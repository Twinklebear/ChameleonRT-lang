#pragma once

#include <memory>
#include "crtl_dxr_export.h"
#include "dxr_buffer.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT BufferView : public crtl::APIObject, public Resource {
    std::shared_ptr<Buffer> buffer;
    CRTL_DATA_TYPE data_type;

    size_t offset_bytes;
    size_t size;

    uint8_t *mapping;
    CRTL_BUFFER_MAP_MODE mapped_mode;

public:
    BufferView(std::shared_ptr<Buffer> buffer,
               CRTL_DATA_TYPE data_type,
               size_t offset_bytes,
               size_t size);

    void *map(CRTL_BUFFER_MAP_MODE map_mode);

    void unmap();

    size_t size_bytes();
};
}
}
