#include "resolver_visitor.h"
#include "ast/visitor.h"
#include "error_listener.h"
#include "parallel_hashmap/phmap.h"

namespace crtl {

ResolverVisitor::VariableStatus::VariableStatus(ast::decl::Variable *var_decl)
    : var_decl(var_decl)
{
}

std::shared_ptr<ResolverPassResult> resolved = std::make_shared<ResolverPassResult>();

std::any ResolverVisitor::visit_ast(ast::AST *ast) {}

std::any ResolverVisitor::visit_decl_function(ast::decl::Function *d)
{
    functions[d->get_token()->getText()] = d;
    // TODO: Also must begin a new scope here for the function parameters
    visit_children(d);
    // Then here end the scope
    return std::any();
}
std::any ResolverVisitor::visit_decl_entry_point(ast::decl::EntryPoint *d) {}
std::any ResolverVisitor::visit_decl_global_param(ast::decl::GlobalParam *d) {}
std::any ResolverVisitor::visit_decl_struct(ast::decl::Struct *d) {}
std::any ResolverVisitor::visit_decl_struct_member(ast::decl::StructMember *d) {}
std::any ResolverVisitor::visit_decl_variable(ast::decl::Variable *d) {}

std::any ResolverVisitor::visit_stmt_block(ast::stmt::Block *s) {}
std::any ResolverVisitor::visit_stmt_if_else(ast::stmt::IfElse *s) {}
std::any ResolverVisitor::visit_stmt_while(ast::stmt::While *s) {}
std::any ResolverVisitor::visit_stmt_for(ast::stmt::For *s) {}
std::any ResolverVisitor::visit_stmt_return(ast::stmt::Return *s) {}
std::any ResolverVisitor::visit_stmt_variable_declaration(ast::stmt::VariableDeclaration *s) {}
std::any ResolverVisitor::visit_stmt_expression(ast::stmt::Expression *s) {}

std::any ResolverVisitor::visit_expr_unary(ast::expr::Unary *e) {}
std::any ResolverVisitor::visit_expr_binary(ast::expr::Binary *e) {}
std::any ResolverVisitor::visit_expr_variable(ast::expr::Variable *e) {}
std::any ResolverVisitor::visit_expr_constant(ast::expr::Constant *e) {}
std::any ResolverVisitor::visit_expr_function_call(ast::expr::FunctionCall *e) {}
std::any ResolverVisitor::visit_struct_array_access(ast::expr::StructArrayAccess *e) {}
std::any ResolverVisitor::visit_expr_assignment(ast::expr::Assignment *e) {}

void ResolverVisitor::begin_scope() {}

void ResolverVisitor::end_scope() {}

void ResolverVisitor::declare(const ast::decl::Variable *var) {}

void ResolverVisitor::define(const ast::decl::Variable *var) {}

ast::decl::Struct *ResolverVisitor::resolve_struct(const ast::ty::Struct *node) {}

ast::decl::Variable *ResolverVisitor::resolve_variable(const ast::expr::Variable *node) {}

ast::decl::Function *ResolverVisitor::resolve_function(const ast::expr::FunctionCall *node) {}
}

