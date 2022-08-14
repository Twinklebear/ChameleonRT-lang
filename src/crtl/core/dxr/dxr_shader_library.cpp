#include "dxr_shader_library.h"
#include <algorithm>
#include <array>
#include <dxcapi.h>
#include "error.h"
#include "util.h"

namespace crtl {
namespace dxr {

using Microsoft::WRL::ComPtr;

ShaderLibrary::ShaderLibrary(const std::string &crtl_src)
    // TODO: CRTL Compiler library needs some way for us to get errors and report them
    // back up to the application
    : crtl_compilation_result(hlsl::compile_crtl(crtl_src))
{
    std::cout << "----\nInput CRTL shader:\n" << crtl_src << "\n";
    std::cout << "Compiled HLSL src:\n"
              << crtl_compilation_result->hlsl_src << "\n----\n";

    // Now compile the HLSL to DXIL
    compile_dxil();

    bytecode.pShaderBytecode = shader_dxil->GetBufferPointer();
    bytecode.BytecodeLength = shader_dxil->GetBufferSize();

    // TODO: Should also build some intermediate struct here instead of working directly
    // on the JSON data in the rest of the code to reduce amount of string
    // manipulation/allocation on the hot code path

    // Build the list of shader entry points
    for (const auto &entry_pt : crtl_compilation_result->shader_info["entry_points"]) {
        std::cout << "entry pt name: " << entry_pt["name"].get<std::string>() << "\n";
        exported_functions.push_back(utf8_to_utf16(entry_pt["name"].get<std::string>()));
    }

    // We need to get the export info translated over from the JSON metadata
    build_library_desc();
}

const D3D12_DXIL_LIBRARY_DESC *ShaderLibrary::library_desc() const
{
    return &dxil_library_desc;
}

void ShaderLibrary::compile_dxil()
{
    ComPtr<IDxcCompiler3> dxc;
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxc));

    std::array<LPCWSTR, 3> dxc_args = {L"-T", L"lib_6_3", L"-O3"};

    DxcBuffer hlsl_src_buf = {};
    hlsl_src_buf.Ptr = crtl_compilation_result->hlsl_src.c_str();
    hlsl_src_buf.Size = crtl_compilation_result->hlsl_src.size();
    hlsl_src_buf.Encoding = DXC_CP_UTF8;

    ComPtr<IDxcResult> dxc_results;
    dxc->Compile(&hlsl_src_buf,
                 dxc_args.data(),
                 dxc_args.size(),
                 nullptr,
                 IID_PPV_ARGS(&dxc_results));

    // Check for compilation errors
    ComPtr<IDxcBlobUtf8> error_log = nullptr;
    dxc_results->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&error_log), nullptr);

    std::string error_log_utf8;
    if (error_log && error_log->GetStringLength() > 0) {
        error_log_utf8 = error_log->GetStringPointer();
        // TODO : do need a way to report warnings in the native shader but probably just
        // for internal development/logging?
        std::cout << "DXC Warnings/Errors: " << error_log_utf8 << "\n";
    }

    HRESULT compilation_status;
    dxc_results->GetStatus(&compilation_status);
    if (FAILED(compilation_status)) {
        throw Error(error_log_utf8, CRTL_ERROR_NATIVE_SHADER_COMPILATION_FAILED);
    }

    // Get the shader binary
    dxc_results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_dxil), nullptr);
    if (!shader_dxil) {
        std::cout << "Somehow didn't get shader dxil!?";
        throw Error("Didn't get DXIL?", CRTL_ERROR_NATIVE_SHADER_COMPILATION_FAILED);
    }
}

nlohmann::json ShaderLibrary::get_entry_point_info(const std::string &entry_point) const
{
    const auto &entry_points = crtl_compilation_result->shader_info["entry_points"];
    const auto fnd = entry_points.find(entry_point);
    if (fnd == entry_points.end()) {
        throw Error("ShaderLibrary does not contain entry point " + entry_point,
                    CRTL_ERROR_ENTRY_POINT_NOT_FOUND);
    }
    return *fnd;
}

void ShaderLibrary::build_library_desc()
{
    for (const auto &fn : exported_functions) {
        D3D12_EXPORT_DESC shader_export = {};
        shader_export.ExportToRename = nullptr;
        shader_export.Flags = D3D12_EXPORT_FLAG_NONE;
        shader_export.Name = fn.c_str();
        exports.push_back(shader_export);
        export_fcn_ptrs.push_back(fn.c_str());
    }
    dxil_library_desc.DXILLibrary = bytecode;
    dxil_library_desc.NumExports = exports.size();
    dxil_library_desc.pExports = exports.data();
}

ShaderEntryPoint::ShaderEntryPoint(const std::string &entry_point_name,
                                   const std::shared_ptr<ShaderLibrary> &shader_library)
    : shader_library(shader_library),
      entry_point_info(shader_library->get_entry_point_info(entry_point_name))
{
}
}
}
