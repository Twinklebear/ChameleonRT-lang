#pragma once

#include <string>
#include <vector>
#include "parallel_hashmap/phmap.h"

namespace crtl {
namespace hlsl {
enum class ShaderRegisterType {
    SHADER_RESOURCE_VIEW,
    SAMPLER,
    UNORDERED_ACCESS_VIEW,
    CONSTANT_BUFFER_VIEW,
    INVALID
};

struct ShaderRegister {
    ShaderRegisterType type = ShaderRegisterType::INVALID;

    int slot = -1;
    // Register space that this register is within
    int space = -1;

    ShaderRegister(ShaderRegisterType type, int slot, int space);

    ShaderRegister() = default;

    std::string to_string() const;
};

struct ParameterRegisterBinding {
    virtual ~ParameterRegisterBinding() = default;
};

// The register binding assigned to an input parameter in HLSL
struct ShaderRegisterBinding : ParameterRegisterBinding {
    ShaderRegister shader_register;
    // The number of registers following the starting slot that the binding occupies,
    // e.g. for arrays of resources. -1 indicates an unbounded size array
    int count = 0;

    ShaderRegisterBinding(ShaderRegister shader_register, int count);

    ShaderRegisterBinding() = default;
};

/* The register binding for a struct parameter whose members may be split
 * over multiple different parameter types. All primitive/vector/matrix members
 * of the struct are packed into a constant buffer, while any buffers, textures, etc.
 * are bound to registers
 */
struct StructRegisterBinding : ParameterRegisterBinding {
    ShaderRegisterBinding constant_buffer_register;
    std::vector<std::string> constant_buffer_contents;

    // Binding info for all non-constant buffer suitable data
    phmap::parallel_flat_hash_map<std::string, ShaderRegisterBinding> members;
};

}
}
