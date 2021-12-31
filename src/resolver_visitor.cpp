#include "resolver_visitor.h"
#include "ast/visitor.h"
#include "error_listener.h"
#include "parallel_hashmap/phmap.h"

namespace crtl {

ResolverVisitor::VariableStatus::VariableStatus(ast::decl::Variable *var_decl)
    : var_decl(var_decl)
{
}

std::any ResolverVisitor::visit_ast(ast::AST *ast)
{
    return std::any();
}

std::any ResolverVisitor::visit_decl_function(ast::decl::Function *d)
{
    functions[d->get_token()->getText()] = d;
    // TODO: Also must begin a new scope here for the function parameters
    visit_children(d);
    // Then here end the scope
    return std::any();
}
std::any ResolverVisitor::visit_decl_entry_point(ast::decl::EntryPoint *d)
{
    return std::any();
}

std::any ResolverVisitor::visit_decl_global_param(ast::decl::GlobalParam *d)
{
    return std::any();
}

std::any ResolverVisitor::visit_decl_struct(ast::decl::Struct *d)
{
    return std::any();
}

std::any ResolverVisitor::visit_decl_struct_member(ast::decl::StructMember *d)
{
    return std::any();
}

std::any ResolverVisitor::visit_decl_variable(ast::decl::Variable *d)
{
    return std::any();
}

std::any ResolverVisitor::visit_stmt_block(ast::stmt::Block *s)
{
    return std::any();
}

std::any ResolverVisitor::visit_stmt_if_else(ast::stmt::IfElse *s)
{
    return std::any();
}

std::any ResolverVisitor::visit_stmt_while(ast::stmt::While *s)
{
    return std::any();
}

std::any ResolverVisitor::visit_stmt_for(ast::stmt::For *s)
{
    return std::any();
}

std::any ResolverVisitor::visit_stmt_return(ast::stmt::Return *s)
{
    return std::any();
}

std::any ResolverVisitor::visit_stmt_variable_declaration(ast::stmt::VariableDeclaration *s)
{
    return std::any();
}

std::any ResolverVisitor::visit_stmt_expression(ast::stmt::Expression *s)
{
    return std::any();
}

std::any ResolverVisitor::visit_expr_unary(ast::expr::Unary *e)
{
    return std::any();
}

std::any ResolverVisitor::visit_expr_binary(ast::expr::Binary *e)
{
    return std::any();
}

std::any ResolverVisitor::visit_expr_variable(ast::expr::Variable *e)
{
    return std::any();
}

std::any ResolverVisitor::visit_expr_constant(ast::expr::Constant *e)
{
    return std::any();
}

std::any ResolverVisitor::visit_expr_function_call(ast::expr::FunctionCall *e)
{
    return std::any();
}

std::any ResolverVisitor::visit_struct_array_access(ast::expr::StructArrayAccess *e)
{
    return std::any();
}

std::any ResolverVisitor::visit_expr_assignment(ast::expr::Assignment *e)
{
    return std::any();
}

void ResolverVisitor::begin_scope()
{
    scopes.emplace_back();
}

void ResolverVisitor::end_scope()
{
    for (auto &v : scopes.back()) {
        if (!v.second.read) {
            auto *token = v.second.var_decl->get_token();
            report_warning(token, "Unused variable '" + token->getText() + "'");
        }
    }
    scopes.pop_back();
}

void ResolverVisitor::declare(const ast::decl::Variable *var)
{
    if (scopes.empty()) {
        return;
    }
    auto &current_scope = scopes.back();
    auto *token = var->get_token();
    auto fnd = current_scope.find(token->getText());
    if (fnd != current_scope.end()) {
        report_error(token, "Variable '" + token->getText() + "' has already been declared");
        return;
    }
    current_scope[token->getText()] = VariableStatus();
}

void ResolverVisitor::define(const ast::decl::Variable *var)
{
    if (scopes.empty()) {
        return;
    }
    auto &current_scope = scopes.back();
    current_scope[var->get_token()->getText()].defined = true;
}

ast::decl::Struct *ResolverVisitor::resolve_struct(const ast::ty::Struct *node)
{
    return nullptr;
}

ast::decl::Variable *ResolverVisitor::resolve_variable(const ast::expr::Variable *node)
{
    return nullptr;
}

ast::decl::Function *ResolverVisitor::resolve_function(const ast::expr::FunctionCall *node)
{
    return nullptr;
}
}
