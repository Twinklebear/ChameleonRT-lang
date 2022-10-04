#include "dxr_shader_record.h"
#include "dxr_shader_record_parameter_block.h"
#include "error.h"

namespace crtl {
namespace dxr {

size_t ShaderRecord::get_parameter_block_size() const
{
    auto rs = get_root_signature();
    return rs ? rs->get_total_size() - D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES : 0;
}

void ShaderRecord::set_parameter_block(
    const std::shared_ptr<ShaderRecordParameterBlock> &parameter_block)
{
    // TODO: This restriction could be lifted somewhat to allow swapping param blocks for
    // shaders with the same parameter layout, but can leave that for later
    if (parameter_block->get_shader_record() != this) {
        throw Error(
            "ShaderRecored: Provided ShaderRecordParameterBlock was not created from "
            "this shader record.",
            CRTL_ERROR_INCOMPATIBLE_SHADER_RECORD_PARAMETER_BLOCK);
    }
    bound_parameter_block = parameter_block;
}

HitGroupRecord::HitGroupRecord(const std::shared_ptr<ShaderEntryPoint> &closest_hit,
                               const std::shared_ptr<ShaderEntryPoint> &intersection,
                               const std::shared_ptr<ShaderEntryPoint> &any_hit)
    : closest_hit(closest_hit), intersection(intersection), any_hit(any_hit)
{
    if (closest_hit->type() != EntryPointType::CLOSEST_HIT) {
        throw Error(
            "HitGroupRecord: Provided closest_hit entry point is not a closest_hit entry "
            "point",
            CRTL_ERROR_INVALID_SHADER_ENTRY_POINT);
    }
    shader_record_name =
        "HG" + std::to_string(next_record_id++) + "_" + closest_hit->name();

    std::cout << "TODO Will: HitGroupRecord parameter entry point matching validation\n";

    if (intersection) {
        if (intersection->type() != EntryPointType::INTERSECTION) {
            throw Error(
                "HitGroupRecord: Provided intersection entry point is not an "
                "intersection entry point",
                CRTL_ERROR_INVALID_SHADER_ENTRY_POINT);
        }
        shader_record_name += "_" + intersection->name();

        hg_type = D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE;
    } else {
        hg_type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
    }

    if (any_hit) {
        if (any_hit->type() != EntryPointType::ANY_HIT) {
            throw Error(
                "HitGroupRecord: Provided any_hit entry point is not an any_hit entry "
                "point",
                CRTL_ERROR_INVALID_SHADER_ENTRY_POINT);
        }
        shader_record_name += "_" + any_hit->name();
    }
}

const phmap::flat_hash_map<std::string, ShaderParameterDesc>
    &HitGroupRecord::get_parameter_info() const
{
    // TODO
    return closest_hit->get_parameter_info();
}

const RootSignature *HitGroupRecord::get_root_signature() const
{
    return nullptr;
}

MissRecord::MissRecord(const std::shared_ptr<ShaderEntryPoint> &entry_point)
    : entry_point(entry_point)
{
    if (entry_point->type() != EntryPointType::MISS) {
        throw Error("MissRecord: Provided miss entry point is not a miss entry point",
                    CRTL_ERROR_INVALID_SHADER_ENTRY_POINT);
    }
    shader_record_name =
        "M" + std::to_string(next_record_id++) + "_" + entry_point->name();
}

const phmap::flat_hash_map<std::string, ShaderParameterDesc>
    &MissRecord::get_parameter_info() const
{
    return entry_point->get_parameter_info();
}

const RootSignature *MissRecord::get_root_signature() const
{
    return entry_point->get_root_signature();
}

RaygenRecord::RaygenRecord(const std::shared_ptr<ShaderEntryPoint> &entry_point)
    : entry_point(entry_point)
{
    if (entry_point->type() != EntryPointType::RAY_GEN) {
        throw Error(
            "RaygenRecord: Provided ray_gen entry point is not a ray_gen entry point",
            CRTL_ERROR_INVALID_SHADER_ENTRY_POINT);
    }
    shader_record_name =
        "RG" + std::to_string(next_record_id++) + "_" + entry_point->name();
}

const phmap::flat_hash_map<std::string, ShaderParameterDesc>
    &RaygenRecord::get_parameter_info() const
{
    return entry_point->get_parameter_info();
}

const RootSignature *RaygenRecord::get_root_signature() const
{
    return entry_point->get_root_signature();
}
}
}
