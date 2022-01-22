#include "shader_register_binding.h"
#include <stdexcept>

namespace crtl {
namespace hlsl {

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
    std::string str = "register(";
    switch (type) {
    case ShaderRegisterType::SHADER_RESOURCE_VIEW:
        str += "t";
        break;
    case ShaderRegisterType::SAMPLER:
        str += "s";
        break;
    case ShaderRegisterType::UNORDERED_ACCESS_VIEW:
        str += "u";
        break;
    case ShaderRegisterType::CONSTANT_BUFFER_VIEW:
        str += "b";
        break;
    default:
        throw std::runtime_error("Cannot generate string for invalid register type");
        break;
    }
    if (slot == -1 || space == -1) {
        throw std::runtime_error("Cannot generate string for invalid register type");
    }

    str += std::to_string(slot) + ", space" + std::to_string(space) + ")";
    return str;
}

}
}
