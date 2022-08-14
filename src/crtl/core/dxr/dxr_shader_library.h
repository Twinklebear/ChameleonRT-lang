#pragma once

#include <string>
#include <vector>
#include "api_object.h"
#include "crtl_dxr_export.h"
#include "dxr_utils.h"
#include "hlsl/crtl_to_hlsl.h"

#include <dxcapi.h>

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT ShaderLibrary : public APIObject {
    std::shared_ptr<hlsl::ShaderCompilationResult> crtl_compilation_result;

    Microsoft::WRL::ComPtr<IDxcBlob> shader_dxil = nullptr;

    D3D12_SHADER_BYTECODE bytecode = {};
    D3D12_DXIL_LIBRARY_DESC dxil_library_desc = {};

    std::vector<std::wstring> exported_functions;
    // A bit annoying but we keep this around too b/c we need a contiguous
    // array of pointers for now to build the exports association in the pipeline
    std::vector<LPCWSTR> export_fcn_ptrs;
    std::vector<D3D12_EXPORT_DESC> exports;

public:
    ShaderLibrary(const std::string &crtl_src);

    ShaderLibrary(const ShaderLibrary &) = delete;
    ShaderLibrary &operator=(const ShaderLibrary &) = delete;

    // TODO: make this not work on JSON
    nlohmann::json get_entry_point_info(const std::string &entry_point) const;

    const D3D12_DXIL_LIBRARY_DESC *library_desc() const;

private:
    void compile_dxil();

    void build_library_desc();
};

// TODO: Shader entry point class can be in here, likely just a string + ref to the
// library

class CRTL_DXR_EXPORT ShaderEntryPoint : public APIObject {
    std::shared_ptr<ShaderLibrary> shader_library;

    // TODO: Make this not JSON so it's not so expensive to access in hot code paths,
    // reduce string alloc & other ops
    // This also doesn't need to copy it from the shader library, it can just reference it
    // by ptr
    nlohmann::json entry_point_info;

public:
    ShaderEntryPoint(const std::string &entry_point_name,
                     const std::shared_ptr<ShaderLibrary> &shader_library);
};
}
}
