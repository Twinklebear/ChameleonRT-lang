#pragma once

#include "shader_library.h"

namespace crtl {
class Device {
public:
    virtual ~Device() = default;

    // TODO: Resource creation APIs go here
    // Buffers, textures, shaders, queues, bind groups, etc..
    std::shared_ptr<ShaderLibrary> create_shader_library(const std::string &src);
};
}
