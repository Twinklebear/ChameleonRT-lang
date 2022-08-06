#pragma once

#include "api_object.h"
#include "crtl/crtl_enums.h"
#include "crtl_dxr_export.h"
#include "dxr_device.h"
#include "dxr_resource.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT Buffer : public crtl::APIObject, public Resource {
    size_t size_bytes = 0;
    D3D12_RESOURCE_FLAGS res_flags = D3D12_RESOURCE_FLAG_NONE;

public:
    Buffer(DXRDevice *device,
           CRTL_MEMORY_SPACE memory_space,
           CRTL_BUFFER_USAGE usages,
           size_t size_bytes);

    size_t size() const;

private:
    D3D12_RESOURCE_DESC res_desc();
};
}
}
