#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "dxr_buffer.h"
#include "dxr_root_signature.h"
#include "dxr_shader_library.h"
#include "dxr_shader_record.h"
#include "dxr_texture.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

struct ShaderPayloadConfig {
    std::vector<std::wstring> functions;
    D3D12_RAYTRACING_SHADER_CONFIG desc;

    ShaderPayloadConfig() = default;
    ShaderPayloadConfig(const std::vector<std::wstring> &functions,
                        uint32_t max_payload_size,
                        uint32_t max_attrib_size);
};

class RTPipeline;

class RTPipelineBuilder {
    std::vector<ShaderLibrary> shader_libs;
    std::wstring ray_gen;
    std::vector<std::wstring> miss_shaders;
    std::vector<HitGroup> hit_groups;
    std::vector<ShaderPayloadConfig> payload_configs;
    std::vector<RootSignatureAssociation> signature_associations;
    RootSignature global_sig;
    uint32_t recursion_depth = 1;

public:
    RTPipelineBuilder &add_shader_library(const ShaderLibrary &library);

    RTPipelineBuilder &set_ray_gen(const std::wstring &ray_gen);

    // Add a miss shader to the pipeline
    RTPipelineBuilder &add_miss_shader(const std::wstring &miss_fn);

    // Add a hitgroup to the pipeline
    RTPipelineBuilder &add_hit_group(const HitGroup &hg);

    RTPipelineBuilder &configure_shader_payload(
        const std::vector<std::wstring> &functions,
        uint32_t max_payload_size,
        uint32_t max_attrib_size);

    RTPipelineBuilder &set_max_recursion(uint32_t depth);

    RTPipelineBuilder &set_shader_root_sig(const std::vector<std::wstring> &functions,
                                           const RootSignature &sig);

    RTPipelineBuilder &set_global_root_sig(const RootSignature &sig);

    RTPipeline create(ID3D12Device5 *device);

private:
    bool has_global_root_sig() const;
    size_t compute_num_subobjects(size_t &num_export_associations,
                                  size_t &num_associated_fcns) const;
};

class RTPipeline {
    RootSignature rt_global_sig;
    Microsoft::WRL::ComPtr<ID3D12StateObject> state;
    ID3D12StateObjectProperties *pipeline_props = nullptr;

    std::wstring ray_gen;
    std::vector<std::wstring> miss_shaders;
    std::vector<std::wstring> hit_groups;
    std::vector<RootSignatureAssociation> signature_associations;

    D3D12_DISPATCH_RAYS_DESC dispatch_desc = {0};
    Buffer cpu_shader_table, shader_table;
    std::unordered_map<std::wstring, size_t> record_offsets;
    uint8_t *sbt_mapping = nullptr;

    friend class RTPipelineBuilder;

    RTPipeline(D3D12_STATE_OBJECT_DESC &desc,
               RootSignature &global_sig,
               const std::wstring &ray_gen,
               const std::vector<std::wstring> &miss_shaders,
               const std::vector<std::wstring> &hit_groups,
               const std::vector<RootSignatureAssociation> &signature_associations,
               ID3D12Device5 *device);

public:
    RTPipeline() = default;

    void map_shader_table();
    void unmap_shader_table();
    // Upload the shader table contents to the default heap
    void upload_shader_table(ID3D12GraphicsCommandList4 *cmd_list);

    // Get the pointer in the table to a specific shader record. The table must be mapped
    uint8_t *shader_record(const std::wstring &shader);

    /* Get the local root signature assigned to the shader, if any. Returns null
     * if no local root signature was set for the shader
     */
    const RootSignature *shader_signature(const std::wstring &shader) const;

    D3D12_DISPATCH_RAYS_DESC dispatch_rays(const glm::uvec2 &img_dims);

    bool has_global_root_sig() const;
    ID3D12RootSignature *global_sig();

    ID3D12StateObject *operator->();
    ID3D12StateObject *get();

private:
    size_t compute_shader_record_size(const std::wstring &shader) const;
};

}
}
