#include "dxr_shader_record.h"
#include "error.h"

namespace crtl {
namespace dxr {

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

}
}
