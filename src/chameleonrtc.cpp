#include <fstream>
#include <iostream>
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
#include "hlsl/output_visitor.h"
#include "json_visitor.h"
#include "rename_entry_point_param_visitor.h"
#include "resolver_visitor.h"

const std::string DIVIDER = "----------\n";

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: chameleonrtc [file]\n";
        return 1;
    }

    antlr4::ANTLRFileStream input;
    input.loadFromFile(argv[1]);

    crtl::ErrorListener error_listener;

    crtg::ChameleonRTLexer lexer(&input);
    lexer.addErrorListener(&error_listener);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();

    if (error_listener.had_error()) {
        std::cout << "Lexer error occured, exiting\n";
        return 1;
    }

    for (const auto &t : tokens.getTokens()) {
        std::cerr << t->toString() << "\n";
    }
    std::cout << DIVIDER;

    crtg::ChameleonRTParser parser(&tokens);
    parser.addErrorListener(&error_listener);
    antlr4::tree::ParseTree *tree = parser.file();

    if (error_listener.had_error()) {
        std::cout << "Parser error occured, exiting\n";
        return 1;
    }

    std::cerr << tree->toStringTree(&parser) << "\n" << DIVIDER;

    crtl::ASTBuilderVisitor ast_builder;
    ast_builder.visit(tree);
    if (ast_builder.had_error) {
        std::cout << "AST builder error occured, exiting\n";
        return 1;
    }

    std::cout << "AST built\n" << DIVIDER;

    auto ast = ast_builder.ast;

    crtl::JSONVisitor json_visitor;
    json_visitor.visit_ast(ast);

    std::cout << "AST JSON:\n" << json_visitor.ast_json.dump(4) << "\n";

    auto builtins = crtl::get_builtin_decls();

    crtl::ResolverVisitor resolver_visitor(builtins);
    resolver_visitor.visit_ast(ast);

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
        return 1;
    }

    crtl::GlobalStructParamExpansionVisitor global_struct_param_expansion_visitor(
        resolver_visitor.resolved);
    ast = std::any_cast<std::shared_ptr<crtl::ast::AST>>(
        global_struct_param_expansion_visitor.visit_ast(ast));

    crtl::RenameEntryPointParamVisitor rename_entry_point_params(resolver_visitor.resolved);
    rename_entry_point_params.visit_ast(ast);

#if 0
    {
        // TODO: this will need to handle generated nodes (null token)
        crtl::JSONVisitor post_modification_json;
        post_modification_json.visit_ast(ast);

        std::cout << "AST JSON post-modifications:\n" << json_visitor.ast_json.dump(4) << "\n";
    }
#endif

    crtl::hlsl::OutputVisitor hlsl_translator(resolver_visitor.resolved);
    const std::string hlsl_src = std::any_cast<std::string>(hlsl_translator.visit_ast(ast));
    std::cout << "CRTL shader translated to HLSL:\n" << hlsl_src << "\n";

    return 0;
}
