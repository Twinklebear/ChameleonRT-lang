#include "parameter_metadata_output_visitor.h"

namespace crtl {
namespace hlsl {

using namespace ast;

ParameterMetadataOutputVisitor::ParameterMetadataOutputVisitor(
    const std::shared_ptr<ResolverPassResult> &resolver_result,
    const std::shared_ptr<ParameterTransforms> &param_transforms,
    const phmap::parallel_flat_hash_map<std::shared_ptr<ast::decl::Variable>,
                                        std::shared_ptr<ParameterRegisterBinding>>
        &param_bindings)
    : resolver_result(resolver_result),
      param_transforms(param_transforms),
      parameter_bindings(param_bindings)
{
}

std::any ParameterMetadataOutputVisitor::visit_ast(const std::shared_ptr<ast::AST> &ast)
{
    nlohmann::json param_metadata;
    for (auto &n : ast->top_level_decls) {
        if (n->get_node_type() != NodeType::DECL_ENTRY_POINT &&
            n->get_node_type() != NodeType::DECL_GLOBAL_PARAM) {
            continue;
        }

        auto json_data = std::any_cast<nlohmann::json>(visit(n));
        if (n->get_node_type() == NodeType::DECL_ENTRY_POINT) {
            param_metadata["entry_points"][n->get_text()] = json_data;
        } else {
            param_metadata["global_params"][n->get_text()] = json_data;
        }
    }

    /* We also need to record info about the expanded global params so that the runtime can map
     * them to the final output parameters properly
     */
    for (const auto &eg : param_transforms->expanded_global_params) {
        nlohmann::json expanded_global_json;
        expanded_global_json["name"] = eg.first->get_text();
        for (const auto &m : eg.second->members) {
            expanded_global_json["members"][m.first] = m.second->get_text();
        }

        param_metadata["expanded_globals"][eg.first->get_text()] = expanded_global_json;
    }

    return param_metadata;
}

std::any ParameterMetadataOutputVisitor::visit_decl_entry_point(
    const std::shared_ptr<ast::decl::EntryPoint> &d)
{
    auto type = std::dynamic_pointer_cast<ty::EntryPoint>(d->get_type());
    nlohmann::json metadata;
    metadata["name"] = d->get_text();
    metadata["type"] = ty::to_string(type->entry_point_type);

    for (const auto &p : d->parameters) {
        nlohmann::json param_json;
        const std::string source_name = param_transforms->renamed_vars[p];
        param_json["source_name"] = source_name;
        param_json["output_name"] = p->get_text();
        param_json["type"] = p->get_type()->to_string();
        param_json["is_builtin_type"] = p->get_type()->is_builtin();

        auto binding = parameter_bindings[p];
        // Built in type parameters map to a single register
        // Structs map to multiple registers + a constant buffer
        if (p->get_type()->is_builtin()) {
            auto reg_binding = std::dynamic_pointer_cast<ShaderRegisterBinding>(binding);
            param_json["space"] = reg_binding->shader_register.space;
            param_json["slot"] = reg_binding->shader_register.slot;
            param_json["register_type"] = hlsl::to_string(reg_binding->shader_register.type);
            param_json["count"] = reg_binding->count;
        } else {
            auto struct_ty = std::dynamic_pointer_cast<ty::Struct>(p->get_type());
            auto struct_decl = resolver_result->struct_type[struct_ty];
            auto struct_binding = std::dynamic_pointer_cast<StructRegisterBinding>(binding);
            for (const auto &m : struct_binding->members) {
                nlohmann::json member_json;

                member_json["space"] = m.second.shader_register.space;
                member_json["slot"] = m.second.shader_register.slot;
                member_json["register_type"] = hlsl::to_string(m.second.shader_register.type);
                member_json["count"] = m.second.count;
                member_json["type"] =
                    struct_decl->get_member(m.first)->get_type()->to_string();

                param_json["members"][m.first] = member_json;
            }

            // Record the constant buffer contents
            if (!struct_binding->constant_buffer_contents.empty()) {
                param_json["constant_buffer"]["space"] =
                    struct_binding->constant_buffer_register.shader_register.space;
                param_json["constant_buffer"]["slot"] =
                    struct_binding->constant_buffer_register.shader_register.slot;
                param_json["constant_buffer"]["register_type"] = hlsl::to_string(
                    struct_binding->constant_buffer_register.shader_register.type);

                std::vector<nlohmann::json> constant_buf_content;
                for (const auto &cm : struct_binding->constant_buffer_contents) {
                    nlohmann::json constant_member;
                    const auto m = struct_decl->get_member(cm);

                    constant_member["name"] = cm;
                    constant_member["type"] = m->get_type()->to_string();
                    constant_buf_content.push_back(constant_member);
                }

                param_json["constant_buffer"]["contents"] = constant_buf_content;
            }
        }

        metadata["parameters"][source_name] = param_json;
    }

    return metadata;
}

std::any ParameterMetadataOutputVisitor::visit_decl_global_param(
    const std::shared_ptr<ast::decl::GlobalParam> &d)
{
    nlohmann::json metadata;
    metadata["name"] = d->get_text();
    auto binding = parameter_bindings[d];
    auto reg_binding = std::dynamic_pointer_cast<ShaderRegisterBinding>(binding);
    metadata["space"] = reg_binding->shader_register.space;
    metadata["slot"] = reg_binding->shader_register.slot;
    metadata["register_type"] = hlsl::to_string(reg_binding->shader_register.type);
    metadata["count"] = reg_binding->count;
    metadata["type"] = d->get_type()->to_string();

    return metadata;
}

}
}
