#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "global_struct_param_expansion_visitor.h"
#include "hlsl/output_visitor.h"
#include "hlsl/parameter_metadata_output_visitor.h"
#include "json_visitor.h"
#include "parameter_transforms.h"
#include "rename_entry_point_param_visitor.h"
#include "resolver_visitor.h"
#include "shader_library.h"

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

std::string get_file_content(const std::string &fname)
{
    std::ifstream file{fname};
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << fname << std::endl;
        return "";
    }
    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

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

    const std::string shader_text = get_file_content(source_file);
    if (shader_text.empty()) {
        std::cout << "Failed to read shader or file was empty\n";
        return 1;
    }

    crtl::ShaderLibrary shader_library(shader_text);

    crtl::GlobalStructParamExpansionVisitor global_struct_param_expansion_visitor(
        shader_library.resolver_result);
    shader_library.ast = std::any_cast<std::shared_ptr<crtl::ast::AST>>(
        global_struct_param_expansion_visitor.visit_ast(shader_library.ast));

    crtl::RenameEntryPointParamVisitor rename_entry_point_params(
        shader_library.resolver_result);
    rename_entry_point_params.visit_ast(shader_library.ast);

    // Print out the final AST and resolver info that's being passed to the HLSL translator
    crtl::JSONVisitor json_visitor;
    auto ast_json = std::any_cast<nlohmann::json>(json_visitor.visit_ast(shader_library.ast));
    std::cout << "AST JSON post-modifications:\n" << ast_json.dump(4) << "\n";

    // Print out the resolver data again to validate that it's been updated correctly
    std::cout << "Resolver data post-modifications\n";
    for (const auto &x : shader_library.resolver_result->struct_type) {
        auto decl = std::any_cast<nlohmann::json>(json_visitor.visit(x.second));
        std::cout << "Resolved struct type: " << x.first->name << " to decl:\n"
                  << decl.dump(4) << "\n";
    }

    for (const auto &x : shader_library.resolver_result->var_expr) {
        auto expr = std::any_cast<nlohmann::json>(json_visitor.visit(x.first));
        auto decl = std::any_cast<nlohmann::json>(json_visitor.visit(x.second));
        std::cout << "Resolved var expr:\n"
                  << expr.dump(4) << "\nreferencing var declared:\n"
                  << decl.dump(4) << "\n";
    }

    for (const auto &x : shader_library.resolver_result->call_expr) {
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

    crtl::hlsl::OutputVisitor hlsl_translator(shader_library.resolver_result);
    const std::string hlsl_src =
        std::any_cast<std::string>(hlsl_translator.visit_ast(shader_library.ast));
    if (!output_file.empty()) {
        std::ofstream fout(output_file.c_str());
        fout << hlsl_src;
    } else {
        std::cout << "CRTL shader translated to HLSL:\n" << hlsl_src << "\n";
    }

    std::cout << "Building parameter metadata JSON\n";
    crtl::hlsl::ParameterMetadataOutputVisitor param_metadata_output(
        shader_library.resolver_result, param_transforms, hlsl_translator.parameter_bindings);
    auto param_binding_json =
        std::any_cast<nlohmann::json>(param_metadata_output.visit_ast(shader_library.ast));
    if (!param_data_output_file.empty()) {
        std::ofstream fout(param_data_output_file.c_str());
        fout << param_binding_json.dump(4);
    } else {
        std::cout << param_binding_json.dump(4) << "\n";
    }

    return 0;
}
