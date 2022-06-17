#include "crtl_to_hlsl.h"

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
namespace hlsl {

ShaderCompilationResult::ShaderCompilationResult(const std::string &hlsl_src,
                                                 nlohmann::json &shader_info)
    : hlsl_src(hlsl_src), shader_info(shader_info)
{
}

std::shared_ptr<ShaderCompilationResult> compile_crtl(const std::string &crtl_src)
{
    const std::string DIVIDER(8, '-');
    // TODO: This compilation step needs to be done in the crtl_compiler library
    // so that the ANTLR parts don't need to be looked at by the RHI
    ErrorListener error_listener;

    antlr4::ANTLRInputStream input_stream(crtl_src);

    crtg::ChameleonRTLexer lexer(&input_stream);
    lexer.addErrorListener(&error_listener);

    auto tokens = std::make_shared<antlr4::CommonTokenStream>(&lexer);
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
    auto tree = parser.file();

    if (error_listener.had_error()) {
        std::cout << "Parser error occured, exiting\n";
        throw std::runtime_error("Parser error");
    }

    std::cerr << tree->toStringTree(&parser) << "\n" << DIVIDER;

    ASTBuilderVisitor ast_builder;
    ast_builder.visit(tree);
    if (ast_builder.had_error) {
        std::cout << "AST builder error occured, exiting\n";
        throw std::runtime_error("AST builder error");
    }

    std::cout << "AST built\n" << DIVIDER;

    auto ast = ast_builder.ast;

    JSONVisitor json_visitor;
    auto ast_json = std::any_cast<nlohmann::json>(json_visitor.visit_ast(ast));

    std::cout << "AST JSON:\n" << ast_json.dump(4) << "\n";

    auto builtins = get_builtin_decls();

    ResolverVisitor resolver_visitor(builtins);
    resolver_visitor.visit_ast(ast);

    auto resolver_result = resolver_visitor.resolved;

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

    // TODO: These depend on the target API backend
    GlobalStructParamExpansionVisitor global_struct_param_expansion_visitor(resolver_result);
    ast = std::any_cast<std::shared_ptr<ast::AST>>(
        global_struct_param_expansion_visitor.visit_ast(ast));

    RenameEntryPointParamVisitor rename_entry_point_params(resolver_result);
    rename_entry_point_params.visit_ast(ast);

    auto param_transforms = std::make_shared<ParameterTransforms>(
        global_struct_param_expansion_visitor.expanded_global_params,
        rename_entry_point_params.renamed_vars);

    OutputVisitor hlsl_translator(resolver_visitor.resolved);
    const std::string hlsl_src = std::any_cast<std::string>(hlsl_translator.visit_ast(ast));
    std::cout << "CRTL shader translated to HLSL:\n" << hlsl_src << "\n";

    std::cout << "Building parameter metadata JSON\n";
    ParameterMetadataOutputVisitor param_metadata_output(
        resolver_visitor.resolved, param_transforms, hlsl_translator.parameter_bindings);
    auto param_binding_json =
        std::any_cast<nlohmann::json>(param_metadata_output.visit_ast(ast));
    std::cout << param_binding_json.dump(4) << "\n";

    // TODO: The param binding json will be a lot more than just the param binding info
    return std::make_shared<ShaderCompilationResult>(hlsl_src, param_binding_json);
}

}
}
