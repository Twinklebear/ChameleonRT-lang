#include "dxr_shader_library.h"
#include <algorithm>
#include "util.h"

namespace crtl {
namespace dxr {

using Microsoft::WRL::ComPtr;

ShaderLibrary::ShaderLibrary(const void *code,
                             const size_t code_size,
                             const std::vector<std::wstring> &export_fns)
    : export_functions(export_fns)
{
    bytecode.pShaderBytecode = code;
    bytecode.BytecodeLength = code_size;
    build_library_desc();
}

ShaderLibrary::ShaderLibrary(const ShaderLibrary &other)
    : bytecode(other.bytecode), export_functions(other.export_functions)
{
    build_library_desc();
}

ShaderLibrary &ShaderLibrary::operator=(const ShaderLibrary &other)
{
    bytecode = other.bytecode;
    export_functions = other.export_functions;
    build_library_desc();
    return *this;
}

const std::vector<std::wstring> &ShaderLibrary::export_names() const
{
    return export_functions;
}

size_t ShaderLibrary::num_exports() const
{
    return export_fcn_ptrs.size();
}

LPCWSTR *ShaderLibrary::export_names_ptr()
{
    return export_fcn_ptrs.data();
}

LPCWSTR *ShaderLibrary::find_export(const std::wstring &name)
{
    auto fnd = std::find(export_functions.begin(), export_functions.end(), name);
    if (fnd != export_functions.end()) {
        size_t idx = std::distance(export_functions.begin(), fnd);
        return &export_fcn_ptrs[idx];
    } else {
        return nullptr;
    }
}

const D3D12_DXIL_LIBRARY_DESC *ShaderLibrary::library() const
{
    return &slibrary;
}

void ShaderLibrary::build_library_desc()
{
    for (const auto &fn : export_functions) {
        D3D12_EXPORT_DESC shader_export = {0};
        shader_export.ExportToRename = nullptr;
        shader_export.Flags = D3D12_EXPORT_FLAG_NONE;
        shader_export.Name = fn.c_str();
        exports.push_back(shader_export);
        export_fcn_ptrs.push_back(fn.c_str());
    }
    slibrary.DXILLibrary = bytecode;
    slibrary.NumExports = exports.size();
    slibrary.pExports = exports.data();
}

}
}
