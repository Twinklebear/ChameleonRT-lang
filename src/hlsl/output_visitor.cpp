#include "output_visitor.h"
#include <memory>
#include "shader_register_binding.h"

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
    std::string hlsl_src;
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
    std::string hlsl_src;
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
        auto binding = std::make_shared<StructRegisterBinding>();
        const auto *struct_decl = dynamic_cast<const decl::Struct *>(param);
        for (const auto &m : struct_decl->members) {
            // Primitive/Vector/Matrix types get packed into a constant buffer
            const auto member_ty = m->get_type();
            const std::string &name = m->get_text();
            if (member_ty->base_type == ty::BaseType::PRIMITIVE ||
                member_ty->base_type == ty::BaseType::VECTOR ||
                member_ty->base_type == ty::BaseType::MATRIX) {
                binding->constant_buffer_contents.push_back(name);
            } else {
                binding->members[name] = bind_builtin_type_parameter(member_ty);
            }
        }
        if (!binding->constant_buffer_contents.empty()) {
            binding->constant_buffer_register = register_allocator.bind_cbv(1);
        }
        parameter_binding[param->get_text()] = binding;

        // TODO: here need to map built in type -> HLSL string
        // to build the constant buffer and the members HLSL source
    } else {
        auto binding =
            std::make_shared<ShaderRegisterBinding>(bind_builtin_type_parameter(param_type));
        parameter_binding[param->get_text()] = binding;

        // TODO: here need to map built in type -> HLSL string
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
        throw std::runtime_error("Unsupported parameter type");
    }
    return ShaderRegisterBinding();
}
}
}
