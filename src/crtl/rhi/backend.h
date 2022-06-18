#pragma once

#include <memory>
#include "device.h"

namespace crtl {

enum class BackendAPI { DX12, VULKAN, METAL, OPTIX, EMBREE };

class Backend {
public:
    virtual ~Backend() = default;

    std::shared_ptr<Device> create_device();
};

std::shared_ptr<Backend> load_backend(const BackendAPI backend_api);

}
