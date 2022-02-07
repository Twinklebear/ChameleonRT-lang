#include "modifying_visitor.h"

namespace crtl {
namespace ast {

// TODO

std::any ModifyingVisitor::visit_ast(const std::shared_ptr<AST> &ast) {}

std::any ModifyingVisitor::visit_decl_function(const std::shared_ptr<decl::Function> &d) {}

std::any ModifyingVisitor::visit_decl_entry_point(const std::shared_ptr<decl::EntryPoint> &d)
{
}

std::any ModifyingVisitor::visit_decl_global_param(const std::shared_ptr<decl::GlobalParam> &d)
{
}

std::any ModifyingVisitor::visit_decl_struct(const std::shared_ptr<decl::Struct> &d) {}

std::any ModifyingVisitor::visit_decl_struct_member(
    const std::shared_ptr<decl::StructMember> &d)
{
}

std::any ModifyingVisitor::visit_decl_variable(const std::shared_ptr<decl::Variable> &d) {}

std::any ModifyingVisitor::visit_stmt_block(const std::shared_ptr<stmt::Block> &s) {}

std::any ModifyingVisitor::visit_stmt_if_else(const std::shared_ptr<stmt::IfElse> &s) {}

std::any ModifyingVisitor::visit_stmt_while(const std::shared_ptr<stmt::While> &s) {}

std::any ModifyingVisitor::visit_stmt_for(const std::shared_ptr<stmt::For> &s) {}

std::any ModifyingVisitor::visit_stmt_return(const std::shared_ptr<stmt::Return> &s) {}

std::any ModifyingVisitor::visit_stmt_variable_declaration(
    const std::shared_ptr<stmt::VariableDeclaration> &s)
{
}

std::any ModifyingVisitor::visit_stmt_expression(const std::shared_ptr<stmt::Expression> &s) {}

std::any ModifyingVisitor::visit_expr_unary(const std::shared_ptr<expr::Unary> &e) {}

std::any ModifyingVisitor::visit_expr_binary(const std::shared_ptr<expr::Binary> &e) {}

std::any ModifyingVisitor::visit_expr_variable(const std::shared_ptr<expr::Variable> &e) {}

std::any ModifyingVisitor::visit_expr_constant(const std::shared_ptr<expr::Constant> &e) {}

std::any ModifyingVisitor::visit_expr_function_call(
    const std::shared_ptr<expr::FunctionCall> &e)
{
}

std::any ModifyingVisitor::visit_struct_array_access(
    const std::shared_ptr<expr::StructArrayAccess> &e)
{
}

std::any ModifyingVisitor::visit_expr_assignment(const std::shared_ptr<expr::Assignment> &e) {}
}
}
