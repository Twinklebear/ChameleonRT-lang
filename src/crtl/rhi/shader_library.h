#pragma once

#include <memory>
#include <string>
#include <vector>

namespace crtl {
class ShaderLibrary {
public:
    ShaderLibrary() = default;
    ShaderLibrary(const std::string &shader_string);

    // ShaderEntryPoint get_entry_point(const std::string &name);
};
}
