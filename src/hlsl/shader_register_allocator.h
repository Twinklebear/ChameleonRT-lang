#pragma once

#include <string>
#include <vector>
#include "ast/type.h"
#include "shader_register_binding.h"

namespace crtl {
namespace hlsl {

/* The ShaderRegisterAllocator tracks which is the next shader slot and space available for
 * each type of shader register, and can bind numbers of items to these registers. Binding
 * items advances the register state
 */
struct ShaderRegisterAllocator {
private:
    ShaderRegister next_srv_reg =
        ShaderRegister(ShaderRegisterType::SHADER_RESOURCE_VIEW, 0, 0);
    ShaderRegister next_sampler_reg = ShaderRegister(ShaderRegisterType::SAMPLER, 0, 0);
    ShaderRegister next_uav_reg =
        ShaderRegister(ShaderRegisterType::UNORDERED_ACCESS_VIEW, 0, 0);
    ShaderRegister next_cbv_reg =
        ShaderRegister(ShaderRegisterType::CONSTANT_BUFFER_VIEW, 0, 0);

public:
    ShaderRegisterBinding bind_srv(int count);

    ShaderRegisterBinding bind_srv_unsized();

    ShaderRegisterBinding bind_sampler(int count);

    ShaderRegisterBinding bind_sampler_unsized();

    ShaderRegisterBinding bind_uav(int count);

    ShaderRegisterBinding bind_uav_unsized();

    ShaderRegisterBinding bind_cbv(int count);

    ShaderRegisterBinding bind_cbv_unsized();
};
}
}
