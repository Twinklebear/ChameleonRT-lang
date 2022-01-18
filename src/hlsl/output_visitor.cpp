#include "output_visitor.h"

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
    std::string hlsl_src;
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
}
}
