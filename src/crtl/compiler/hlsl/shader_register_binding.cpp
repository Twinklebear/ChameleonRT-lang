#include "shader_register_binding.h"
#include <stdexcept>

namespace crtl {
namespace hlsl {

const std::string to_string(const ShaderRegisterType &sr_type)
{
    switch (sr_type) {
    case ShaderRegisterType::SHADER_RESOURCE_VIEW:
        return "t";
    case ShaderRegisterType::SAMPLER:
        return "s";
    case ShaderRegisterType::UNORDERED_ACCESS_VIEW:
        return "u";
    case ShaderRegisterType::CONSTANT_BUFFER_VIEW:
        return "b";
    default:
        throw std::runtime_error("Cannot generate string for invalid register type");
        return "";
    }
}

ShaderRegister::ShaderRegister(ShaderRegisterType type, int slot, int space)
    : type(type), slot(slot), space(space)
{
}

ShaderRegisterBinding::ShaderRegisterBinding(ShaderRegister shader_register, int count)
    : shader_register(shader_register), count(count)
{
}

std::string ShaderRegister::to_string() const
{
    if (slot == -1 || space == -1) {
        throw std::runtime_error("Cannot generate string for invalid register type");
    }
    return "register(" + hlsl::to_string(type) + std::to_string(slot) + ", space" +
           std::to_string(space) + ")";
}

}
}
