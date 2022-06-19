#pragma once

#include "shader_library.h"

namespace crtl {
/* A Device is an API + HW that we will execute rendering on,
 * e.g. DX12 + a GPU, Embree + a CPU, etc.
 */
class Device {
public:
    virtual ~Device() = default;

    // TODO: Resource creation APIs go here
    // Buffers, textures, shaders, queues, bind groups, etc..
    std::shared_ptr<ShaderLibrary> create_shader_library(const std::string &src);
};
}
