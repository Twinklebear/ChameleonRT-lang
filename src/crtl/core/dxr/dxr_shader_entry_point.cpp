#include "dxr_shader_entry_point.h"
#include <algorithm>
#include <array>
#include <dxcapi.h>
#include "dxr_shader_library.h"
#include "error.h"
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

    // TODO: Decide on and build here (or in shader library step) some non-json faster
    // representation of parameter info and other entry point info

    // Build the local root signature for the entry point
    root_signature = RootSignatureBuilder::build_local_from_desc(
        sbt_parameter_info(), device->get_d3d12_device().Get());
}

EntryPointType ShaderEntryPoint::type() const
{
    return entry_point_type;
}

const std::string &ShaderEntryPoint::name() const
{
    return entry_point_name;
}

nlohmann::json ShaderEntryPoint::sbt_parameter_info() const
{
    // TODO: I'll rework how hit groups work + how params for entry points are sent
    // in the JSON metadata, we just assume there's only one param since in the future
    // this will be enforced by the compiler
    return *entry_point_info["parameters"].begin();
}

}
}
