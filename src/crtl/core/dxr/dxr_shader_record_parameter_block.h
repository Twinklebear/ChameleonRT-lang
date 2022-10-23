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

    void set_parameter(const std::string &name,
                       CRTL_DATA_TYPE data_type,
                       void *parameter) override;

    void set_parameter(const std::string &name,
                       CRTL_DATA_TYPE data_type,
                       const std::shared_ptr<BufferView> &parameter) override;
    /*
     * TODO: acceleration structures as parameters as well
     */

    size_t size() const;

    const ShaderRecord *get_shader_record() const;
};
}
}
