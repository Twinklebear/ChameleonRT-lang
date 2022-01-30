#include "output_visitor.h"
#include <memory>
#include "shader_register_binding.h"
#include "translate_builtin_function_call.h"
#include "translate_builtin_type.h"

namespace crtl {
namespace hlsl {

using namespace ast;

OutputVisitor::OutputVisitor(const std::shared_ptr<ResolverPassResult> &resolver_result)
    : resolver_result(resolver_result)
{
}

std::any OutputVisitor::visit_ast(ast::AST *ast)
{
    std::string hlsl_src = "// CRTL HLSL Output\n";
    for (auto &n : ast->top_level_decls) {
        hlsl_src += std::any_cast<std::string>(visit(n.get())) + "\n";
    }
    return hlsl_src;
}

std::any OutputVisitor::visit_decl_function(ast::decl::Function *d)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_decl_entry_point(ast::decl::EntryPoint *d)
{
    // TODO: Entry point parameters need to be renamed to avoid name collision in
    // a pass that works on the AST. Not at this point when we're just doing the lowering
    // to HLSL
    // Translate the entry point's parameters into shader input parameters for HLSL
    std::string hlsl_src;
    for (auto &p : d->parameters) {
        hlsl_src += bind_parameter(p.get()) + "\n";
    }

    // Emit the entry point declaration, then translate the entry point function body
    auto ty = std::dynamic_pointer_cast<ty::EntryPoint>(d->get_type());
    if (ty->entry_point_type != ty::EntryPointType::COMPUTE) {
        hlsl_src += "[shader(\"";
        switch (ty->entry_point_type) {
        case ty::EntryPointType::RAY_GEN:
            hlsl_src += "raygeneration";
            break;
        case ty::EntryPointType::CLOSEST_HIT:
            hlsl_src += "closesthit";
            break;
        case ty::EntryPointType::ANY_HIT:
            hlsl_src += "anyhit";
            break;
        case ty::EntryPointType::INTERSECTION:
            hlsl_src += "intersection";
            break;
        case ty::EntryPointType::MISS:
            hlsl_src += "miss";
            break;
        default:
            report_error(d->get_token(), "Unhandled RT shader entry point type!");
            break;
        }
        hlsl_src += "\")]\n";
    } else {
        report_error(
            d->get_token(),
            "TODO WILL: Compute shaders need to take & translate num threads annotation");
    }

    // Emit the declaration
    hlsl_src += "void " + d->get_text() + "()\n";

    // Emit the function body
    hlsl_src += std::any_cast<std::string>(visit(d->block.get()));

    return hlsl_src;
}

std::any OutputVisitor::visit_decl_global_param(ast::decl::GlobalParam *d)
{
    std::string hlsl_src = bind_parameter(d);
    return hlsl_src;
}

std::any OutputVisitor::visit_decl_struct(ast::decl::Struct *d)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_decl_struct_member(ast::decl::StructMember *d)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_decl_variable(ast::decl::Variable *d)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_stmt_block(ast::stmt::Block *s)
{
    std::string hlsl_src = "{\n";

    // Visit the block's statements and append them to the HLSL output
    auto stmts = std::any_cast<std::vector<std::any>>(visit_children(s));
    for (auto &stmt : stmts) {
        hlsl_src += std::any_cast<std::string>(stmt) + "\n";
    }

    hlsl_src += "}\n";
    return hlsl_src;
}

std::any OutputVisitor::visit_stmt_if_else(ast::stmt::IfElse *s)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_stmt_while(ast::stmt::While *s)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_stmt_for(ast::stmt::For *s)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_stmt_return(ast::stmt::Return *s)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_stmt_variable_declaration(ast::stmt::VariableDeclaration *s)
{
    std::string hlsl_src;
    auto var_decl = s->var_decl;
    if (var_decl->get_type()->base_type != ty::BaseType::STRUCT) {
        hlsl_src = translate_builtin_type(var_decl->get_type());
    } else {
        hlsl_src = var_decl->get_type()->to_string();
    }
    hlsl_src += " " + var_decl->get_text();
    if (var_decl->expression) {
        auto expr = std::any_cast<std::string>(visit(var_decl->expression.get()));
        hlsl_src += " = " + expr;
    }
    hlsl_src += ";";
    return hlsl_src;
}

std::any OutputVisitor::visit_stmt_expression(ast::stmt::Expression *s)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_expr_unary(ast::expr::Unary *e)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_expr_binary(ast::expr::Binary *e)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_expr_variable(ast::expr::Variable *e)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_expr_constant(ast::expr::Constant *e)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_expr_function_call(ast::expr::FunctionCall *e)
{
    std::string hlsl_src;
    auto *callee = resolver_result->call_expr[e];

    // Translate calls to built-ins to the appropriate built in HLSL function
    if (callee->is_builtin()) {
        hlsl_src = translate_builtin_function_call(e, callee);
        if (hlsl_src.empty()) {
            report_error(e->get_token(), "Unhandled built-in call!");
        }
    } else {
        hlsl_src += e->get_text() + "(";
        auto args = std::any_cast<std::vector<std::any>>(visit_children(e));
        for (size_t i = 0; i < args.size(); ++i) {
            hlsl_src += std::any_cast<std::string>(args[i]);
            if (i + 1 < args.size()) {
                hlsl_src += ", ";
            }
        }
        hlsl_src += ")";
    }
    return hlsl_src;
}

std::any OutputVisitor::visit_struct_array_access(ast::expr::StructArrayAccess *e)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::any OutputVisitor::visit_expr_assignment(ast::expr::Assignment *e)
{
    std::string hlsl_src;
    return hlsl_src;
}

std::string OutputVisitor::bind_parameter(const ast::decl::Variable *param)
{
    std::string hlsl_src;
    const auto param_type = param->get_type();
    if (param_type->base_type == ty::BaseType::STRUCT) {
        const auto *struct_decl =
            resolver_result->struct_type[dynamic_cast<ty::Struct *>(param_type.get())];
        if (!struct_decl) {
            report_error(param->get_token(),
                         "Error: Failed to find resolved struct decl for struct var decl");
            return "";
        }

        auto binding = std::make_shared<StructRegisterBinding>();
        const std::string struct_name = param->get_text();
        for (const auto &m : struct_decl->members) {
            // Primitive/Vector/Matrix types get packed into a constant buffer
            const auto member_ty = m->get_type();
            const std::string &name = m->get_text();
            if (member_ty->base_type == ty::BaseType::PRIMITIVE ||
                member_ty->base_type == ty::BaseType::VECTOR ||
                member_ty->base_type == ty::BaseType::MATRIX) {
                binding->constant_buffer_contents.push_back(name);
            } else if (member_ty->base_type == ty::BaseType::STRUCT) {
                // If we have another struct type member we need to expand it out to flatten
                // the structs down
                // TODO: Maybe this is best done as a pre-pass on the AST that does this
                // flattening of the types, then we don't need to worry about it at this point
                report_error(param->get_token(),
                             "TODO Will: Nested structs in global/entry point param");
            } else {
                binding->members[name] = bind_builtin_type_parameter(member_ty);
            }
        }
        if (!binding->constant_buffer_contents.empty()) {
            binding->constant_buffer_register = register_allocator.bind_cbv(1);
        }
        parameter_binding[param->get_text()] = binding;

        std::string cbuffer_src;
        if (!binding->constant_buffer_contents.empty()) {
            cbuffer_src = "cbuffer " + struct_name + "_cbv : " +
                          binding->constant_buffer_register.shader_register.to_string() +
                          "{\n";
        }
        for (const auto &m : struct_decl->members) {
            const auto member_ty = m->get_type();
            const std::string &name = m->get_text();

            const std::string type_str = translate_builtin_type(member_ty);
            if (member_ty->base_type == ty::BaseType::PRIMITIVE ||
                member_ty->base_type == ty::BaseType::VECTOR ||
                member_ty->base_type == ty::BaseType::MATRIX) {
                // Rename the cbuffer members to StructParamName_MemberName
                // TODO: This should be based on a slightly different naming, maybe do a
                // pre-pass renaming the structs for entry points to avoid name collisions
                cbuffer_src += "\t" + type_str + " " + struct_name + "_" + name + ";\n";

            } else if (member_ty->base_type != ty::BaseType::STRUCT) {
                const auto reg = binding->members[name];
                // Rename the members to StructParamName_MemberName
                // TODO: This should be based on a slightly different naming, maybe do a
                // pre-pass renaming the structs for entry points to avoid name collisions
                hlsl_src += type_str + " " + struct_name + "_" + name + " : " +
                            reg.shader_register.to_string() + ";\n";
            }
        }

        // Generate the constant buffer, if we have one
        if (!cbuffer_src.empty()) {
            cbuffer_src += "}\n";
            hlsl_src += cbuffer_src;
        }
    } else {
        auto binding =
            std::make_shared<ShaderRegisterBinding>(bind_builtin_type_parameter(param_type));
        parameter_binding[param->get_text()] = binding;

        const std::string type_str = translate_builtin_type(param->get_type());
        hlsl_src = type_str + " " + param->get_text() + " : " +
                   binding->shader_register.to_string() + ";";
    }
    return hlsl_src;
}

ShaderRegisterBinding OutputVisitor::bind_builtin_type_parameter(
    const std::shared_ptr<ast::ty::Type> &type)
{
    // TODO: Need to add array (and unsized) declaration support for array and unsized array
    // params
    if (type->base_type == ty::BaseType::PRIMITIVE ||
        type->base_type == ty::BaseType::VECTOR || type->base_type == ty::BaseType::MATRIX) {
        return register_allocator.bind_cbv(1);
    } else if (type->base_type == ty::BaseType::BUFFER) {
        // Buffer is SRV
        // RWBuffer is UAV
        const auto buf_ty = std::dynamic_pointer_cast<ty::Buffer>(type);
        if (buf_ty->access == ty::Access::READ_ONLY) {
            return register_allocator.bind_srv(1);
        } else {
            return register_allocator.bind_uav(1);
        }
    } else if (type->base_type == ty::BaseType::TEXTURE) {
        // Texture is SRV + needs a sampler
        // TODO: Samplers should be added to the built in types
        // RWTexture is UAV
        const auto tex_ty = std::dynamic_pointer_cast<ty::Texture>(type);
        if (tex_ty->access == ty::Access::READ_ONLY) {
            return register_allocator.bind_srv(1);
        } else {
            return register_allocator.bind_uav(1);
        }
    } else if (type->base_type == ty::BaseType::ACCELERATION_STRUCTURE) {
        // AccelerationStructure is SRV
        return register_allocator.bind_srv(1);
    } else {
        throw std::runtime_error("Unsupported parameter type: '" + type->to_string() + "'");
    }
    return ShaderRegisterBinding();
}
}
}
