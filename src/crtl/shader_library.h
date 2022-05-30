#pragma once

#include <memory>
#include <string>
#include <vector>
#include "compiler/ast/node.h"
#include "compiler/resolver_visitor.h"
#include "shader_entry_point.h"

namespace crtl {
class ShaderLibrary {
public:
    ShaderLibrary() = default;
    ShaderLibrary(const std::string &shader_string);

    // ShaderEntryPoint get_entry_point(const std::string &name);

    // private:

    // TODO: Only public for testing
    antlr4::ANTLRInputStream input_stream;
    std::shared_ptr<antlr4::CommonTokenStream> tokens;
    antlr4::tree::ParseTree *tree = nullptr;

    std::shared_ptr<ast::AST> ast;
    std::shared_ptr<ResolverPassResult> resolver_result;
};
}
