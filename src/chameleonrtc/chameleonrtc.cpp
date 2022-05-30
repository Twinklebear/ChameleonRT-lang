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
#include "hlsl/parameter_metadata_output_visitor.h"
#include "json_visitor.h"
#include "parameter_transforms.h"
#include "rename_entry_point_param_visitor.h"
#include "resolver_visitor.h"

const std::string USAGE =
    R"(Usage:
    ./chameleonrtc <file.crtl> [options]

Options:
    -t (hlsl)       Set the compilation target. Only HLSL for now
    -o <out.ext>    Output filename
    -m <out.json>   Parameter metadata output filename, used by the ChameleonRT runtime
    -h              Print this information
)";

const std::string DIVIDER = "----------\n";

int main(int argc, char **argv)
{
    const std::vector<std::string> args(argv + 1, argv + argc);
    if (args.empty() || std::find(args.begin(), args.end(), std::string("-h")) != args.end()) {
        std::cerr << USAGE << "\n";
    }

    const std::string source_file = args[0];
    std::string output_file;
    std::string param_data_output_file;
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "-t") {
            if (args[++i] != "hlsl") {
                std::cout << "Only HLSL is supported right now\n";
            }
        } else if (args[i] == "-o") {
            output_file = args[++i];
        } else if (args[i] == "-m") {
            param_data_output_file = args[++i];
        } else {
            std::cerr << "Unhandled argument ;" << args[i] << "'\n";
        }
    }

    antlr4::ANTLRFileStream input;
    input.loadFromFile(source_file);

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
    auto ast_json = std::any_cast<nlohmann::json>(json_visitor.visit_ast(ast));

    std::cout << "AST JSON:\n" << ast_json.dump(4) << "\n";

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

    // Print out the final AST and resolver info that's being passed to the HLSL translator
    ast_json = std::any_cast<nlohmann::json>(json_visitor.visit_ast(ast));
    std::cout << "AST JSON post-modifications:\n" << ast_json.dump(4) << "\n";

    // Print out the resolver data again to validate that it's been updated correctly
    std::cout << "Resolver data post-modifications\n";
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

    auto param_transforms = std::make_shared<crtl::ParameterTransforms>(
        global_struct_param_expansion_visitor.expanded_global_params,
        rename_entry_point_params.renamed_vars);

    crtl::hlsl::OutputVisitor hlsl_translator(resolver_visitor.resolved);
    const std::string hlsl_src = std::any_cast<std::string>(hlsl_translator.visit_ast(ast));
    if (!output_file.empty()) {
        std::ofstream fout(output_file.c_str());
        fout << hlsl_src;
    } else {
        std::cout << "CRTL shader translated to HLSL:\n" << hlsl_src << "\n";
    }

    std::cout << "Building parameter metadata JSON\n";
    crtl::hlsl::ParameterMetadataOutputVisitor param_metadata_output(
        resolver_visitor.resolved, param_transforms, hlsl_translator.parameter_bindings);
    auto param_binding_json =
        std::any_cast<nlohmann::json>(param_metadata_output.visit_ast(ast));
    if (!param_data_output_file.empty()) {
        std::ofstream fout(param_data_output_file.c_str());
        fout << param_binding_json.dump(4);
    } else {
        std::cout << param_binding_json.dump(4) << "\n";
    }

    return 0;
}
