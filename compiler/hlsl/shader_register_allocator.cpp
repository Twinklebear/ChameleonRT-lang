#include "shader_register_allocator.h"

namespace crtl {
namespace hlsl {

ShaderRegisterBinding ShaderRegisterAllocator::bind_srv(int count)
{
    ShaderRegisterBinding binding(next_srv_reg, count);
    next_srv_reg.slot += count;
    return binding;
}

ShaderRegisterBinding ShaderRegisterAllocator::bind_srv_unsized()
{
    ShaderRegisterBinding binding(next_srv_reg, -1);
    next_srv_reg.space++;
    next_srv_reg.slot = 0;
    return binding;
}

ShaderRegisterBinding ShaderRegisterAllocator::bind_sampler(int count)
{
    ShaderRegisterBinding binding(next_sampler_reg, count);
    next_sampler_reg.slot += count;
    return binding;
}

ShaderRegisterBinding ShaderRegisterAllocator::bind_sampler_unsized()
{
    ShaderRegisterBinding binding(next_sampler_reg, -1);
    next_sampler_reg.space++;
    next_sampler_reg.slot = 0;
    return binding;
}

ShaderRegisterBinding ShaderRegisterAllocator::bind_uav(int count)
{
    ShaderRegisterBinding binding(next_uav_reg, count);
    next_uav_reg.slot += count;
    return binding;
}

ShaderRegisterBinding ShaderRegisterAllocator::bind_uav_unsized()
{
    ShaderRegisterBinding binding(next_uav_reg, -1);
    next_uav_reg.space++;
    next_uav_reg.slot = 0;
    return binding;
}

ShaderRegisterBinding ShaderRegisterAllocator::bind_cbv(int count)
{
    ShaderRegisterBinding binding(next_cbv_reg, count);
    next_cbv_reg.slot += count;
    return binding;
}

ShaderRegisterBinding ShaderRegisterAllocator::bind_cbv_unsized()
{
    ShaderRegisterBinding binding(next_cbv_reg, -1);
    next_cbv_reg.space++;
    next_cbv_reg.slot = 0;
    return binding;
}

}
}
