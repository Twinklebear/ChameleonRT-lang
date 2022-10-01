#pragma once
#pragma once

#include <string>
#include <vector>
#include "api_object.h"
#include "crtl_dxr_export.h"
#include "dxr_device.h"
#include "dxr_root_signature.h"
#include "dxr_shader_library.h"
#include "dxr_shader_parameter_desc.h"
#include "dxr_utils.h"
#include "hlsl/crtl_to_hlsl.h"
#include "parallel_hashmap/phmap.h"

#include <dxcapi.h>

namespace crtl {
namespace dxr {

enum class EntryPointType {
    RAY_GEN,
    CLOSEST_HIT,
    ANY_HIT,
    INTERSECTION,
    MISS,
    COMPUTE,
    // TODO: Callable shaders?
    INVALID
};

class CRTL_DXR_EXPORT ShaderEntryPoint : public APIObject {
    std::shared_ptr<ShaderLibrary> shader_library;

    // TODO: Make this not JSON so it's not so expensive to access in hot code paths,
    // reduce string alloc & other ops
    // This also doesn't need to copy it from the shader library, it can just reference it
    // by ptr
    nlohmann::json entry_point_info = nullptr;
    EntryPointType entry_point_type = EntryPointType::INVALID;

    phmap::flat_hash_map<std::string, ShaderParameterDesc> parameter_info;

    // TODO: this would be lifted into the entry point metadata when I move off JSON for
    // storing it
    std::string entry_point_name;

    std::shared_ptr<RootSignature> root_signature;

public:
    ShaderEntryPoint(DXRDevice *dxrdevice,
                     const std::string &entry_point_name,
                     const std::shared_ptr<ShaderLibrary> &shader_library);

    EntryPointType type() const;

    const std::string &name() const;

    const phmap::flat_hash_map<std::string, ShaderParameterDesc> &get_parameter_info()
        const;

    const RootSignature *get_root_signature() const;

private:
    // Builds the root signature and the parameter info
    void build_root_signature(DXRDevice *device);
};
}
}
