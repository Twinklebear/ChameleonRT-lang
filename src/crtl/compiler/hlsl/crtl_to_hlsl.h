#pragma once

#include <memory>
#include <string>
#include "json.hpp"

namespace crtl {
namespace hlsl {

struct ShaderCompilationResult {
    std::string hlsl_src;
    nlohmann::json shader_info;
};

std::shared_ptr<ShaderCompilationResult> compile_crtl(const std::string &crtl_src);
}
}
