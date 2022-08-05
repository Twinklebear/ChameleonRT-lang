#pragma once

#include <string>
#include <vector>
#include "crtl_dxr_export.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT ShaderLibrary {
    D3D12_SHADER_BYTECODE bytecode = {0};
    D3D12_DXIL_LIBRARY_DESC slibrary = {0};

    std::vector<std::wstring> export_functions;
    // A bit annoying but we keep this around too b/c we need a contiguous
    // array of pointers for now to build the exports association in the pipeline
    // TODO: We don't need to keep this
    std::vector<LPCWSTR> export_fcn_ptrs;
    std::vector<D3D12_EXPORT_DESC> exports;

public:
    ShaderLibrary(const void *bytecode,
                  const size_t bytecode_size,
                  // TODO: This exports list will have to come from the CRTL compilation
                  // step, it will have this information
                  const std::vector<std::wstring> &exports);

    ShaderLibrary(const ShaderLibrary &other);

    ShaderLibrary &operator=(const ShaderLibrary &other);

    const std::vector<std::wstring> &export_names() const;

    size_t num_exports() const;

    LPCWSTR *export_names_ptr();

    LPCWSTR *find_export(const std::wstring &name);

    const D3D12_DXIL_LIBRARY_DESC *library() const;

private:
    void build_library_desc();
};

// TODO: Shader entry point class can be in here, likely just a string + ref to the
// library
}
}
