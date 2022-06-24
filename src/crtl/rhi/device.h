#pragma once

#include "api_object.h"
#include "crtl/crtl_enums.h"
#include "shader_library.h"

namespace crtl {
/* A Device is an API + HW that we will execute rendering on,
 * e.g. DX12 + a GPU, Embree + a CPU, etc.
 */
class Device : public APIObject {
public:
    virtual ~Device() = default;

    virtual DEVICE_API device_api() const = 0;

    // TODO: Resource creation APIs go here
    // Buffers, textures, shaders, queues, bind groups, etc..
    virtual std::shared_ptr<ShaderLibrary> create_shader_library(const std::string &src) = 0;
};
}
