#include "shader_library.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "ChameleonRTLexer.h"
#include "ChameleonRTParser.h"
#include "antlr4-runtime.h"
#include "ast/modifying_visitor.h"
#include "ast_builder_visitor.h"
#include "builtins.h"
#include "error_listener.h"
#include "global_struct_param_expansion_visitor.h"
#include "json_visitor.h"
#include "parameter_transforms.h"
#include "rename_entry_point_param_visitor.h"
#include "resolver_visitor.h"

#include "hlsl/output_visitor.h"
#include "hlsl/parameter_metadata_output_visitor.h"

namespace crtl {

ShaderLibrary::ShaderLibrary(const std::string &shader_string) : input_stream(shader_string)
{
    const std::string DIVIDER = "----------\n";

    crtl::ErrorListener error_listener;

    crtg::ChameleonRTLexer lexer(&input_stream);
    lexer.addErrorListener(&error_listener);

    tokens = std::make_shared<antlr4::CommonTokenStream>(&lexer);
    tokens->fill();

    if (error_listener.had_error()) {
        std::cout << "Lexer error occured, exiting\n";
        throw std::runtime_error("Lexer error");
    }

    for (const auto &t : tokens->getTokens()) {
        std::cerr << t->toString() << "\n";
    }
    std::cout << DIVIDER;

    crtg::ChameleonRTParser parser(tokens.get());
    parser.addErrorListener(&error_listener);
    tree = parser.file();

    if (error_listener.had_error()) {
        std::cout << "Parser error occured, exiting\n";
        throw std::runtime_error("Parser error");
    }

    std::cerr << tree->toStringTree(&parser) << "\n" << DIVIDER;

    crtl::ASTBuilderVisitor ast_builder;
    ast_builder.visit(tree);
    if (ast_builder.had_error) {
        std::cout << "AST builder error occured, exiting\n";
        throw std::runtime_error("AST builder error");
    }

    std::cout << "AST built\n" << DIVIDER;

    ast = ast_builder.ast;

    crtl::JSONVisitor json_visitor;
    auto ast_json = std::any_cast<nlohmann::json>(json_visitor.visit_ast(ast));

    std::cout << "AST JSON:\n" << ast_json.dump(4) << "\n";

    auto builtins = crtl::get_builtin_decls();

    crtl::ResolverVisitor resolver_visitor(builtins);
    resolver_visitor.visit_ast(ast);

    resolver_result = resolver_visitor.resolved;

    // For testing, print out some info about what was resolved by the resolver
    for (const auto &x : resolver_visitor.resolved->struct_type) {
        auto decl = std::any_cast<nlohmann::json>(json_visitor.visit(x.second));
        std::cout << "Resolved struct type: " << x.first->name << " to decl:\n"
                  << decl.dump(4) << "\n";
    }

    for (const auto &x : resolver_visitor.resolved->var_expr) {
        auto expr = std::any_cast<nlohmann::json>(json_visitor.visit(x.first));
        auto decl = std::any_cast<nlohmann::json>(json_visitor.visit(x.second));
        std::cout << "Resolved var expr:\n"
                  << expr.dump(4) << "\nreferencing var declared:\n"
                  << decl.dump(4) << "\n";
    }

    for (const auto &x : resolver_visitor.resolved->call_expr) {
        auto expr = std::any_cast<nlohmann::json>(json_visitor.visit(x.first));
        std::cout << "Resolved function call:\n"
                  << expr.dump(4) << "\nto function declared:\n";
        if (!x.second->is_builtin()) {
            auto decl = std::any_cast<nlohmann::json>(json_visitor.visit(x.second));
            std::cout << decl.dump(4) << "\n";
        } else {
            std::cout << "Built-in function: " << x.second->get_text() << "\n";
        }
    }

    if (resolver_visitor.had_error) {
        std::cout << "Error during resolver pass, exiting\n";
        throw std::runtime_error("Resolver error");
    }
}
}
