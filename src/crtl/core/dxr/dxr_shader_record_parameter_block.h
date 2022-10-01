#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>
#include "crtl_dxr_export.h"
#include "dxr_blas.h"
#include "dxr_buffer_view.h"
#include "dxr_parameter_block.h"
#include "dxr_shader_record.h"
#include "dxr_texture.h"
#include "dxr_tlas.h"
#include "error.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT ShaderRecordParameterBlock : public ParameterBlock {
    std::shared_ptr<ShaderRecord> shader_record;

    // The parameter block data is populated here and just memcpy'd into the SBT
    std::vector<uint8_t> parameter_block;

public:
    // TODO: This will basically just provide a way to set values for the elements in
    // the shader entry point's phmap::flat_hash_map<std::string, ShaderParameterDesc>
    // parameter_info;
    ShaderRecordParameterBlock(const std::shared_ptr<ShaderRecord> &shader_record);

    template <typename T,
              typename = std::enable_if_t<std::is_standard_layout<T>::value, bool> = true>
    void set_parameter(const std::string &name,
                       CRTL_DATA_TYPE data_type,
                       const T *parameter);

    void set_parameter(const std::string &name,
                       CRTL_DATA_TYPE data_type,
                       const std::shared_ptr<BufferView> &parameter);
    /*
     * TODO: acceleration structures as parameters as well
     */

    size_t size() const;
};

template <typename T,
          typename = std::enable_if_t<std::is_standard_layout<T>::value, bool> = true>
void ShaderRecordParameterBlock::set_parameter(const std::string &name,
                                               CRTL_DATA_TYPE data_type,
                                               const T *parameter)
{
    try {
        // offset will throw if there's no sbt_constants
        const size_t sbt_constants_offset =
            entry_point->get_root_signature()->offset("sbt_constants");
    } catch (const std::runtime_error &e) {
        throw Error("Parameter " + name + " does not exist in the parameter block.",
                    CRTL_ERROR_INVALID_PARAMETER_NAME);
    }
    const auto &parameter_info = entry_point->get_parameter_info();
    auto param_info = parameter_info.find(name);
    if (param_info == parameter_info.end()) {
        throw Error("Parameter " + name + " does not exist in parameter block.",
                    CRTL_ERROR_INVALID_PARAMETER_NAME);
    }

    const size_t param_offset =
        sbt_constants_offset + param_info->second.constant_offset_bytes;
    std::memcpy(parameter_block.data() + param_offset, parameter, sizeof(T));
}
}
}
