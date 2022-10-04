#include "dxr_shader_entry_point.h"
#include <algorithm>
#include <array>
#include <dxcapi.h>
#include "dxr_shader_library.h"
#include "error.h"
#include "type.h"
#include "util.h"

namespace crtl {
namespace dxr {

using Microsoft::WRL::ComPtr;
ShaderEntryPoint::ShaderEntryPoint(DXRDevice *device,
                                   const std::string &entry_point_name,
                                   const std::shared_ptr<ShaderLibrary> &shader_library)
    : shader_library(shader_library),
      entry_point_info(shader_library->get_entry_point_info(entry_point_name)),
      entry_point_name(entry_point_info["name"])
{
    const std::string ty = entry_point_info["type"];
    if (ty == "RAY_GEN") {
        entry_point_type = EntryPointType::RAY_GEN;
    } else if (ty == "MISS") {
        entry_point_type = EntryPointType::MISS;
    } else {
        // Note: hit groups will be handled differently
        std::cout << "TODO support for entry point type: " << ty << "\n";
        throw Error("TODO support for entry point type " + ty, CRTL_ERROR_UNKNOWN);
    }
    // TODO: This would need to be done later for hit group records within the hit group
    // shader record.
    build_root_signature(device);
}

EntryPointType ShaderEntryPoint::type() const
{
    return entry_point_type;
}

const std::string &ShaderEntryPoint::name() const
{
    return entry_point_name;
}

const phmap::flat_hash_map<std::string, ShaderParameterDesc>
    &ShaderEntryPoint::get_parameter_info() const
{
    return parameter_info;
}

const RootSignature *ShaderEntryPoint::get_root_signature() const
{
    return root_signature.get();
}

void ShaderEntryPoint::build_root_signature(DXRDevice *device)
{
    auto builder = RootSignatureBuilder::local();
    auto &inline_constants = entry_point_info["parameters"]["params"]["constant_buffer"];
    if (!inline_constants.is_null()) {
        auto &constants_arr = inline_constants["contents"];

        // TODO: just testing here but this type info should be computed in the shader
        // entry point to build its param desc in some non-JSON faster representation
        // Note: it's enforced at the language level that only primitives, vectors and
        // matrices can be in the constants list here

        // TODO: Maybe rework the constants a bit more to treat them like a user-struct?
        // Then the shader param desc would be a constant buffer for a single struct item,
        // wouldn't have the redundant slot/space stored on each constant param
        const uint32_t constants_slot = inline_constants["slot"].get<int>();
        const uint32_t constants_space = inline_constants["space"].get<int>();

        uint32_t num_constants = 0;
        for (auto &c : constants_arr) {
            auto ty = ty::parse_type(c["type"]);
            // TODO: Compiler I think should be making sure all these names are unique at
            // this point across the different params or emitted a compile error. At least
            // the HLSL compiler would give an error, but CRTL compiler should do that
            // before hand
            std::cout << "const name: " << c["name"].get<std::string>() << "\n";
            parameter_info[c["name"].get<std::string>()] =
                ShaderParameterDesc(ShaderParameterType::INLINE_CONSTANT,
                                    ty,
                                    constants_slot,
                                    constants_space,
                                    num_constants * 4);

            num_constants += ty->size() / 4;
        }
        std::cout << "sbt constants:\n  - slot: " << constants_slot << "\n"
                  << "  - # of constants = " << num_constants << "\n"
                  << "  - space: " << constants_space << "\n";
        builder.add_constants(
            "sbt_constants", constants_slot, num_constants, constants_space);
    }

    auto &members = entry_point_info["parameters"]["params"]["members"];
    for (auto &m : members.items()) {
        std::cout << "member: " << m << "\n";
        auto ty = ty::parse_type(m.value()["type"]);
        const uint32_t slot = m.value()["slot"].get<int>();
        const uint32_t space = m.value()["space"].get<int>();

        if (ty->base_type == ty::BaseType::BUFFER_VIEW) {
            auto buf_view = std::dynamic_pointer_cast<ty::BufferView>(ty);
            if (buf_view->access == ty::Access::READ_ONLY) {
                parameter_info[m.key()] = ShaderParameterDesc(
                    ShaderParameterType::SHADER_RESOURCE_VIEW, ty, slot, space);
                builder.add_srv(m.key(), slot, space);
            } else {
                parameter_info[m.key()] = ShaderParameterDesc(
                    ShaderParameterType::UNORDERED_ACCESS_VIEW, ty, slot, space);
                builder.add_uav(m.key(), slot, space);
            }
        } else if (ty->base_type == ty::BaseType::TEXTURE) {
            auto texture = std::dynamic_pointer_cast<ty::Texture>(ty);
            if (texture->access == ty::Access::READ_ONLY) {
                parameter_info[m.key()] = ShaderParameterDesc(
                    ShaderParameterType::SHADER_RESOURCE_VIEW, ty, slot, space);
                builder.add_srv(m.key(), slot, space);
            } else if (texture->access == ty::Access::READ_WRITE) {
                parameter_info[m.key()] = ShaderParameterDesc(
                    ShaderParameterType::UNORDERED_ACCESS_VIEW, ty, slot, space);
                builder.add_uav(m.key(), slot, space);
            }
        } else if (ty->base_type == ty::BaseType::ACCELERATION_STRUCTURE) {
            parameter_info[m.key()] = ShaderParameterDesc(
                ShaderParameterType::SHADER_RESOURCE_VIEW, ty, slot, space);
            builder.add_srv(m.key(), slot, space);
        }
    }

    root_signature = builder.build(device->get_d3d12_device().Get());
}

}
}
