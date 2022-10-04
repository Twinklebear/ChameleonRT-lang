#include "dxr_shader_record_parameter_block.h"
#include "util.h"

namespace crtl {
namespace dxr {
ShaderRecordParameterBlock::ShaderRecordParameterBlock(
    const std::shared_ptr<ShaderRecord> &shader_record)
    : shader_record(shader_record)
{
    parameter_block.resize(shader_record->get_parameter_block_size());
    std::cout << "Parameter block size: " << parameter_block.size() << "\n";
}

void ShaderRecordParameterBlock::set_parameter(const std::string &name,
                                               CRTL_DATA_TYPE data_type,
                                               void *parameter)
{
    const auto &parameter_info = shader_record->get_parameter_info();
    auto param_info = parameter_info.find(name);
    if (param_info == parameter_info.end()) {
        throw Error("Parameter " + name + " does not exist in parameter block.",
                    CRTL_ERROR_INVALID_PARAMETER_NAME);
    }

    try {
        // offset will throw if there's no sbt_constants. The root signature offsets
        // include the shader identifier size, which we don't need to account for when
        // building the parameter block
        // TODO: Would make sense to move the ident size subtraction off higher up (to the
        // shader record?)
        const size_t sbt_constants_offset =
            shader_record->get_root_signature()->offset("sbt_constants") -
            D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        const size_t param_offset =
            sbt_constants_offset + param_info->second.constant_offset_bytes;
        std::cout << "Write param of size " << crtl::data_type_size(data_type) << " "
                  << name << " at offset " << param_offset << " in SBT\n";
        std::memcpy(parameter_block.data() + param_offset,
                    parameter,
                    crtl::data_type_size(data_type));
    } catch (const std::runtime_error &e) {
        throw Error("Parameter " + name + " does not exist in the parameter block.",
                    CRTL_ERROR_INVALID_PARAMETER_NAME);
    }
}

void ShaderRecordParameterBlock::set_parameter(
    const std::string &name,
    CRTL_DATA_TYPE data_type,
    const std::shared_ptr<BufferView> &parameter)
{
    try {
        // offset will throw if the parameter doesn't exist. The root signature offsets
        // include the shader identifier size, which we don't need to account for when
        // building the parameter block
        const size_t param_offset = shader_record->get_root_signature()->offset(name) -
                                    D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        D3D12_GPU_VIRTUAL_ADDRESS gpu_virtual_addr = parameter->gpu_virtual_address();
        std::cout << "Write buffer view param " << name << " at offset " << param_offset
                  << " in SBT\n";
        std::memcpy(parameter_block.data() + param_offset,
                    &gpu_virtual_addr,
                    sizeof(D3D12_GPU_VIRTUAL_ADDRESS));
    } catch (const std::runtime_error &e) {
        throw Error("Parameter " + name + " does not exist in the parameter block.",
                    CRTL_ERROR_INVALID_PARAMETER_NAME);
    }
}

size_t ShaderRecordParameterBlock::size() const
{
    return parameter_block.size();
}

const ShaderRecord *ShaderRecordParameterBlock::get_shader_record() const
{
    return shader_record.get();
}
}
}
