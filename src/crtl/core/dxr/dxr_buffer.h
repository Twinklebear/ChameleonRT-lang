#pragma once

#include "api_object.h"
#include "crtl/crtl_enums.h"
#include "dxr_device.h"
#include "dxr_resource.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class Buffer : public crtl::APIObject, public Resource {
    size_t buf_size = 0;
    D3D12_RESOURCE_FLAGS res_flags = D3D12_RESOURCE_FLAG_NONE;

public:
    Buffer(DXRDevice *device,
           CRTL_MEMORY_SPACE memory_space,
           CRTL_BUFFER_USAGE usages,
           size_t bytes);

    // TODO: Keep? Or go all through the views internally too?
    // Map the whole range for potentially being read
    void *map();

    // Map to read a specific or empty range
    void *map(D3D12_RANGE read);

    // Unmap and mark the whole range as written
    void unmap();

    // Unmap and mark a specific range as written
    void unmap(D3D12_RANGE written);

    size_t size() const;

private:
    D3D12_RESOURCE_DESC res_desc();
};

}
}
