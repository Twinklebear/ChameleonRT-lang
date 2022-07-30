#include "dxr_rtpipeline.h"
#include <algorithm>
#include <cstring>
#include "util.h"

namespace crtl {
namespace dxr {

ShaderPayloadConfig::ShaderPayloadConfig(const std::vector<std::wstring> &functions,
                                         uint32_t max_payload_size,
                                         uint32_t max_attrib_size)
    : functions(functions)
{
    desc.MaxPayloadSizeInBytes = max_payload_size;
    desc.MaxAttributeSizeInBytes = max_attrib_size;
}

RTPipelineBuilder &RTPipelineBuilder::add_shader_library(const ShaderLibrary &library)
{
    shader_libs.push_back(library);
    return *this;
}

RTPipelineBuilder &RTPipelineBuilder::set_ray_gen(const std::wstring &rg)
{
    assert(ray_gen.empty());
    ray_gen = rg;
    return *this;
}

RTPipelineBuilder &RTPipelineBuilder::add_miss_shader(const std::wstring &miss_fn)
{
    miss_shaders.push_back(miss_fn);
    return *this;
}

RTPipelineBuilder &RTPipelineBuilder::add_hit_group(const HitGroup &hg)
{
    hit_groups.push_back({hg});
    return *this;
}

RTPipelineBuilder &RTPipelineBuilder::configure_shader_payload(
    const std::vector<std::wstring> &functions,
    uint32_t max_payload_size,
    uint32_t max_attrib_size)
{
    payload_configs.emplace_back(functions, max_payload_size, max_attrib_size);
    return *this;
}

RTPipelineBuilder &RTPipelineBuilder::set_max_recursion(uint32_t depth)
{
    recursion_depth = depth;
    return *this;
}

RTPipelineBuilder &RTPipelineBuilder::set_shader_root_sig(
    const std::vector<std::wstring> &functions, const RootSignature &sig)
{
    signature_associations.emplace_back(functions, sig);
    return *this;
}

RTPipelineBuilder &RTPipelineBuilder::set_global_root_sig(const RootSignature &sig)
{
    global_sig = sig;
    return *this;
}

RTPipeline RTPipelineBuilder::create(ID3D12Device5 *device)
{
    if (ray_gen.empty()) {
        throw std::runtime_error("No ray generation shader set!");
    }

    size_t num_association_subobjs = 0;
    size_t num_associated_fcns = 0;
    const size_t total_subobjs =
        compute_num_subobjects(num_association_subobjs, num_associated_fcns);

    std::vector<D3D12_STATE_SUBOBJECT> subobjects;
    subobjects.resize(total_subobjs);
    size_t current_obj = 0;

    std::vector<D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION> associations;
    associations.resize(num_association_subobjs);
    size_t current_assoc = 0;

    std::vector<LPCWSTR> associated_fcns;
    associated_fcns.resize(num_associated_fcns, nullptr);
    size_t current_assoc_fcn = 0;

    // Add the shader libraries
    for (const auto &lib : shader_libs) {
        D3D12_STATE_SUBOBJECT l = {0};
        l.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        l.pDesc = lib.library();
        subobjects[current_obj++] = l;
    }

    // Make the hit group descriptors for each hit group and ray type and add them
    std::vector<D3D12_HIT_GROUP_DESC> hg_descs;
    // Names for the RTPipeline to setup the shader table with
    std::vector<std::wstring> hit_group_names;
    if (!hit_groups.empty()) {
        hg_descs.resize(hit_groups.size());
        for (size_t i = 0; i < hit_groups.size(); ++i) {
            const auto &hg = hit_groups[i];

            hit_group_names.push_back(hg.name);

            D3D12_HIT_GROUP_DESC &desc = hg_descs[i];
            desc.HitGroupExport = hg.name.c_str();
            desc.Type = hg.type;
            desc.ClosestHitShaderImport = hg.closest_hit.c_str();
            desc.IntersectionShaderImport =
                hg.has_intersection() ? hg.intersection.c_str() : nullptr;
            desc.AnyHitShaderImport = hg.has_any_hit() ? hg.any_hit.c_str() : nullptr;

            D3D12_STATE_SUBOBJECT o = {0};
            o.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
            o.pDesc = &desc;
            subobjects[current_obj++] = o;
        }
    }

    // Make the shader payload configs and associate them with the desired functions
    for (const auto &c : payload_configs) {
        // Add the shader config object
        D3D12_STATE_SUBOBJECT o = {0};
        o.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
        o.pDesc = &c.desc;
        subobjects[current_obj++] = o;

        // Associate it with the exports
        D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION &assoc = associations[current_assoc++];
        assoc.NumExports = c.functions.size();
        assoc.pExports = &associated_fcns[current_assoc_fcn];
        assoc.pSubobjectToAssociate = &subobjects[current_obj - 1];

        // Copy over the names referenced by this association
        for (const auto &name : c.functions) {
            associated_fcns[current_assoc_fcn++] = name.c_str();
        }

        D3D12_STATE_SUBOBJECT payload_subobj = {0};
        payload_subobj.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
        payload_subobj.pDesc = &assoc;
        subobjects[current_obj++] = payload_subobj;
    }

    // Make the local root signature objects and associations
    std::vector<D3D12_LOCAL_ROOT_SIGNATURE> local_root_sigs;
    if (!signature_associations.empty()) {
        local_root_sigs.resize(signature_associations.size());
        size_t current_sig = 0;
        for (auto &sig : signature_associations) {
            // Add the local root signature
            D3D12_LOCAL_ROOT_SIGNATURE &local_sig = local_root_sigs[current_sig++];
            local_sig.pLocalRootSignature = sig.signature.get();

            D3D12_STATE_SUBOBJECT root_sig_obj = {0};
            root_sig_obj.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
            root_sig_obj.pDesc = &local_sig;
            subobjects[current_obj++] = root_sig_obj;

            // Associate it with the exports
            D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION &assoc = associations[current_assoc++];
            assoc.NumExports = sig.shaders.size();
            assoc.pExports = &associated_fcns[current_assoc_fcn];
            assoc.pSubobjectToAssociate = &subobjects[current_obj - 1];

            // Copy over the names referenced by this association
            for (const auto &name : sig.shaders) {
                associated_fcns[current_assoc_fcn++] = name.c_str();
            }

            D3D12_STATE_SUBOBJECT payload_subobj = {0};
            payload_subobj.Type =
                D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
            payload_subobj.pDesc = &assoc;
            subobjects[current_obj++] = payload_subobj;
        }
    }

    // Add the raytracing pipeline config
    D3D12_RAYTRACING_PIPELINE_CONFIG pipeline_cfg = {0};
    pipeline_cfg.MaxTraceRecursionDepth = 1;
    {
        D3D12_STATE_SUBOBJECT o = {0};
        o.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
        o.pDesc = &pipeline_cfg;
        // 8: Pipeline config
        subobjects[current_obj++] = o;
    }

    // Add the global root signature if we have one
    D3D12_GLOBAL_ROOT_SIGNATURE global_root_sig_obj = {0};
    if (has_global_root_sig()) {
        global_root_sig_obj.pGlobalRootSignature = global_sig.get();
        D3D12_STATE_SUBOBJECT o = {0};
        o.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
        o.pDesc = &global_root_sig_obj;
        subobjects[current_obj++] = o;
    }

    D3D12_STATE_OBJECT_DESC pipeline_desc = {0};
    pipeline_desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
    pipeline_desc.NumSubobjects = current_obj;
    pipeline_desc.pSubobjects = subobjects.data();

    return RTPipeline(pipeline_desc,
                      global_sig,
                      ray_gen,
                      miss_shaders,
                      hit_group_names,
                      signature_associations,
                      device);
}

bool RTPipelineBuilder::has_global_root_sig() const
{
    return global_sig.sig.Get() != nullptr;
}

size_t RTPipelineBuilder::compute_num_subobjects(size_t &num_export_associations,
                                                 size_t &num_associated_fcns) const
{
    // Compute how many state objects we'll need for this pipeline
    // Each DXIL library takes one subobject
    size_t num_subobjs = shader_libs.size();

    // Each hit group takes one subobject
    num_subobjs += hit_groups.size();

    // Each shader payload config takes two subobjects:
    // One to declare the config, and another to associate it with the functions
    num_subobjs += payload_configs.size() * 2;
    num_export_associations = payload_configs.size();
    num_associated_fcns = 0;
    for (const auto &c : payload_configs) {
        num_associated_fcns += c.functions.size();
    }

    // Each local root signature association takes two subobjects:
    // One to declare the subobject, and another to associate it with the functions
    num_subobjs += signature_associations.size() * 2;
    num_export_associations += signature_associations.size();
    for (const auto &a : signature_associations) {
        num_associated_fcns += a.shaders.size();
    }

    // Specifying the max trace recursion depth takes 1 subobject
    ++num_subobjs;

    // If we have a global root signature that takes 1 subobject
    if (has_global_root_sig()) {
        ++num_subobjs;
    }
    return num_subobjs;
}

RTPipeline::RTPipeline(
    D3D12_STATE_OBJECT_DESC &desc,
    RootSignature &global_sig,
    const std::wstring &ray_gen,
    const std::vector<std::wstring> &miss_shaders,
    const std::vector<std::wstring> &hit_groups,
    const std::vector<RootSignatureAssociation> &signature_associations,
    ID3D12Device5 *device)
    : rt_global_sig(global_sig),
      ray_gen(ray_gen),
      miss_shaders(miss_shaders),
      hit_groups(hit_groups),
      signature_associations(signature_associations)
{
    CHECK_ERR(device->CreateStateObject(&desc, IID_PPV_ARGS(&state)));
    CHECK_ERR(state->QueryInterface(&pipeline_props));

    // Compute the offsets/strides for each set of shaders in the SBT
    const size_t raygen_record_offset = 0;
    dispatch_desc.RayGenerationShaderRecord.SizeInBytes =
        compute_shader_record_size(ray_gen);

    const size_t miss_record_offset =
        align_to(dispatch_desc.RayGenerationShaderRecord.SizeInBytes,
                 D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
    dispatch_desc.MissShaderTable.StrideInBytes = 0;
    for (const auto &m : miss_shaders) {
        dispatch_desc.MissShaderTable.StrideInBytes = std::max(
            dispatch_desc.MissShaderTable.StrideInBytes, compute_shader_record_size(m));
    }
    dispatch_desc.MissShaderTable.SizeInBytes =
        dispatch_desc.MissShaderTable.StrideInBytes * miss_shaders.size();

    const size_t hitgroup_record_offset =
        align_to(miss_record_offset + dispatch_desc.MissShaderTable.SizeInBytes,
                 D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
    dispatch_desc.HitGroupTable.StrideInBytes = 0;
    for (const auto &hg : hit_groups) {
        dispatch_desc.HitGroupTable.StrideInBytes = std::max(
            dispatch_desc.HitGroupTable.StrideInBytes, compute_shader_record_size(hg));
    }
    dispatch_desc.HitGroupTable.SizeInBytes =
        dispatch_desc.HitGroupTable.StrideInBytes * hit_groups.size();

    const size_t sbt_size =
        hitgroup_record_offset + dispatch_desc.HitGroupTable.SizeInBytes;

    cpu_shader_table =
        Buffer::upload(device, sbt_size, D3D12_RESOURCE_STATE_GENERIC_READ);
    shader_table = Buffer::device(device, sbt_size, D3D12_RESOURCE_STATE_GENERIC_READ);

    // Build the list of offsets into the shader table for each shader record
    // and write the identifiers into the table. The actual arguments are left to the user
    map_shader_table();

    // Write the ray gen shader
    dispatch_desc.RayGenerationShaderRecord.StartAddress =
        shader_table->GetGPUVirtualAddress();
    record_offsets[ray_gen] = 0;
    std::memcpy(sbt_mapping,
                pipeline_props->GetShaderIdentifier(ray_gen.c_str()),
                D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

    // Write the miss shaders
    dispatch_desc.MissShaderTable.StartAddress =
        shader_table->GetGPUVirtualAddress() + miss_record_offset;
    size_t offset = miss_record_offset;
    for (const auto &m : miss_shaders) {
        record_offsets[m] = offset;
        std::memcpy(sbt_mapping + offset,
                    pipeline_props->GetShaderIdentifier(m.c_str()),
                    D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

        offset += dispatch_desc.MissShaderTable.StrideInBytes;
    }

    // Write the hit group shaders
    dispatch_desc.HitGroupTable.StartAddress =
        shader_table->GetGPUVirtualAddress() + hitgroup_record_offset;
    offset = hitgroup_record_offset;
    for (const auto &hg : hit_groups) {
        record_offsets[hg] = offset;
        std::memcpy(sbt_mapping + offset,
                    pipeline_props->GetShaderIdentifier(hg.c_str()),
                    D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

        offset += dispatch_desc.HitGroupTable.StrideInBytes;
    }
    unmap_shader_table();
}

void RTPipeline::map_shader_table()
{
    assert(sbt_mapping == nullptr);
    sbt_mapping = static_cast<uint8_t *>(cpu_shader_table.map());
}

void RTPipeline::unmap_shader_table()
{
    assert(sbt_mapping);
    cpu_shader_table.unmap();
    sbt_mapping = nullptr;
}

void RTPipeline::upload_shader_table(ID3D12GraphicsCommandList4 *cmd_list)
{
    assert(sbt_mapping == nullptr);

    auto b = barrier_transition(shader_table, D3D12_RESOURCE_STATE_COPY_DEST);
    cmd_list->ResourceBarrier(1, &b);

    cmd_list->CopyResource(shader_table.get(), cpu_shader_table.get());

    b = barrier_transition(shader_table, D3D12_RESOURCE_STATE_GENERIC_READ);
    cmd_list->ResourceBarrier(1, &b);
}

uint8_t *RTPipeline::shader_record(const std::wstring &shader)
{
    assert(sbt_mapping);
    auto fnd = record_offsets.find(shader);
    if (fnd != record_offsets.end()) {
        return sbt_mapping + fnd->second;
    } else {
        throw std::runtime_error("Request for shader record not in table!");
    }
}

const RootSignature *RTPipeline::shader_signature(const std::wstring &shader) const
{
    // Note: The numbers of shaders and root signatures should be relatively small,
    // but this is O(n^2). For a massive scene update this to use a faster unordered map
    // (like the sparsepp one). However, for a big scene the number of signatures
    // shouldn't be too high still right? Just the number of shaders they're associated
    // with
    auto fnd =
        std::find_if(signature_associations.begin(),
                     signature_associations.end(),
                     [&](const RootSignatureAssociation &s) {
                         return std::find(s.shaders.begin(), s.shaders.end(), shader) !=
                                s.shaders.end();
                     });
    if (fnd != signature_associations.end()) {
        return &fnd->signature;
    } else {
        return nullptr;
    }
}

D3D12_DISPATCH_RAYS_DESC RTPipeline::dispatch_rays(const glm::uvec2 &img_dims)
{
    D3D12_DISPATCH_RAYS_DESC dispatch_rays = dispatch_desc;

    dispatch_rays.Width = img_dims.x;
    dispatch_rays.Height = img_dims.y;
    dispatch_rays.Depth = 1;

    return dispatch_rays;
}

bool RTPipeline::has_global_root_sig() const
{
    return rt_global_sig.sig.Get() != nullptr;
}

ID3D12RootSignature *RTPipeline::global_sig()
{
    return rt_global_sig.get();
}

ID3D12StateObject *RTPipeline::operator->()
{
    return get();
}
ID3D12StateObject *RTPipeline::get()
{
    return state.Get();
}

size_t RTPipeline::compute_shader_record_size(const std::wstring &shader) const
{
    size_t shader_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
    // Size of the shader's local root sig, if any
    auto *sig = shader_signature(shader);
    if (sig) {
        shader_size += sig->total_size();
    }
    return align_to(shader_size, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
}

}
}
