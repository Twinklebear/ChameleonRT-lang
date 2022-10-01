#pragma once

#include "dxr_shader_record_parameter_block.h"

namespace crtl {
namespace dxr {
ShaderRecordParameterBlock::ShaderRecordParameterBlock(
    const std::shared_ptr<ShaderRecord> &shader_record)
    : shader_record(shader_record)
{
    parameter_block.resize(shader_record->get_parameter_block_size());
    std::cout << "Parameter block size: " << parameter_block.size() << "\n";
}

void ShaderRecordParameterBlock::set_parameter(
    const std::string &name,
    CRTL_DATA_TYPE data_type,
    const std::shared_ptr<BufferView> &parameter)
{
    try {
        // offset will throw if there's no such parameter
        const size_t param_offset_offset =
            shader_record->get_root_signature()->offset(name);
        D3D12_GPU_VIRTUAL_ADDRESS gpu_virtual_addr = parameter->gpu_virtual_address();
        std::memcpy(
            parameter_block.data(), &gpu_virtual_addr, sizeof(D3D12_GPU_VIRTUAL_ADDRESS));
    } catch (const std::runtime_error &e) {
        throw Error("Parameter " + name + " does not exist in the parameter block.",
                    CRTL_ERROR_INVALID_PARAMETER_NAME);
    }
}

size_t ShaderRecordParameterBlock::size() const
{
    return parameter_block.size();
}
}
}
