#include "modifying_visitor.h"

namespace crtl {
namespace ast {

std::any ModifyingVisitor::visit_ast(const std::shared_ptr<AST> &ast)
{
    std::vector<std::shared_ptr<decl::Declaration>> decls;
    for (auto &n : ast->top_level_decls) {
        collect_results(visit(n), decls);
    }

    auto ast_out = std::make_shared<AST>();
    for (auto &d : decls) {
        ast_out->top_level_decls.push_back(d);
    }

    return ast_out;
}

std::any ModifyingVisitor::visit_decl_function(const std::shared_ptr<decl::Function> &d)
{
    // We have to get back decl::Declaration to be compatible with top level decls
    std::vector<std::shared_ptr<decl::Declaration>> parameters;
    for (auto &p : d->parameters) {
        collect_results(visit(p), parameters);
    }

    // Same compatibility reason for accepting a plain statement type here
    auto block = std::any_cast<std::shared_ptr<stmt::Statement>>(visit(d->block));

    d->parameters.clear();
    for (auto &p : parameters) {
        d->parameters.push_back(std::dynamic_pointer_cast<decl::Variable>(p));
    }
    d->block = std::dynamic_pointer_cast<stmt::Block>(block);
    return std::dynamic_pointer_cast<decl::Declaration>(d);
}

std::any ModifyingVisitor::visit_decl_entry_point(const std::shared_ptr<decl::EntryPoint> &d)
{
    // We have to get back decl::Declaration to be compatible with top level decls
    std::vector<std::shared_ptr<decl::Declaration>> parameters;
    for (auto &p : d->parameters) {
        collect_results(visit(p), parameters);
    }

    // Same compatibility reason for accepting a plain statement type here
    auto block = std::any_cast<std::shared_ptr<stmt::Statement>>(visit(d->block));

    d->parameters.clear();
    for (auto &p : parameters) {
        d->parameters.push_back(std::dynamic_pointer_cast<decl::Variable>(p));
    }
    d->block = std::dynamic_pointer_cast<stmt::Block>(block);
    return std::dynamic_pointer_cast<decl::Declaration>(d);
}

std::any ModifyingVisitor::visit_decl_global_param(const std::shared_ptr<decl::GlobalParam> &d)
{
    return std::dynamic_pointer_cast<decl::Declaration>(d);
}

std::any ModifyingVisitor::visit_decl_struct(const std::shared_ptr<decl::Struct> &d)
{
    std::vector<std::shared_ptr<decl::StructMember>> members;
    for (auto &m : d->members) {
        collect_results(visit(m), members);
    }
    d->members = members;
    return std::dynamic_pointer_cast<decl::Declaration>(d);
}

std::any ModifyingVisitor::visit_decl_struct_member(
    const std::shared_ptr<decl::StructMember> &d)
{
    return d;
}

std::any ModifyingVisitor::visit_decl_variable(const std::shared_ptr<decl::Variable> &d)
{
    d->expression = std::any_cast<std::shared_ptr<expr::Expression>>(d->expression);
    return std::dynamic_pointer_cast<decl::Declaration>(d);
}

std::any ModifyingVisitor::visit_stmt_block(const std::shared_ptr<stmt::Block> &s)
{
    std::vector<std::shared_ptr<stmt::Statement>> statements;
    for (auto &st : s->statements) {
        collect_results(visit(st), statements);
    }
    s->statements = statements;
    return std::dynamic_pointer_cast<stmt::Statement>(s);
}

std::any ModifyingVisitor::visit_stmt_if_else(const std::shared_ptr<stmt::IfElse> &s)
{
    auto condition = std::any_cast<std::shared_ptr<expr::Expression>>(visit(s->condition));
    auto if_branch = std::any_cast<std::shared_ptr<stmt::Statement>>(visit(s->if_branch));

    std::shared_ptr<stmt::Statement> else_branch;
    if (s->else_branch) {
        else_branch = result_or_nullptr<stmt::Statement>(visit(s->else_branch));
    }

    s->condition = condition;
    s->if_branch = if_branch;
    s->else_branch = else_branch;
    return std::dynamic_pointer_cast<stmt::Statement>(s);
}

std::any ModifyingVisitor::visit_stmt_while(const std::shared_ptr<stmt::While> &s)
{
    auto condition = std::any_cast<std::shared_ptr<expr::Expression>>(visit(s->condition));
    std::shared_ptr<stmt::Statement> body;
    if (s->body) {
        body = result_or_nullptr<stmt::Statement>(visit(s->body));
    }

    s->condition = condition;
    s->body = body;
    return std::dynamic_pointer_cast<stmt::Statement>(s);
}

std::any ModifyingVisitor::visit_stmt_for(const std::shared_ptr<stmt::For> &s)
{
    std::shared_ptr<stmt::Statement> init;
    if (s->init) {
        init = result_or_nullptr<stmt::Statement>(visit(s->init));
    }

    std::shared_ptr<expr::Expression> condition;
    if (s->condition) {
        condition = result_or_nullptr<expr::Expression>(visit(s->condition));
    }

    std::shared_ptr<expr::Expression> advance;
    if (s->advance) {
        advance = result_or_nullptr<expr::Expression>(visit(s->advance));
    }

    std::shared_ptr<stmt::Statement> body;
    if (s->body) {
        body = result_or_nullptr<stmt::Statement>(visit(s->body));
    }

    s->init = init;
    s->condition = condition;
    s->advance = advance;
    s->body = body;
    return std::dynamic_pointer_cast<stmt::Statement>(s);
}

std::any ModifyingVisitor::visit_stmt_return(const std::shared_ptr<stmt::Return> &s)
{
    if (s->expression) {
        s->expression = result_or_nullptr<expr::Expression>(visit(s->expression));
    }
    return std::dynamic_pointer_cast<stmt::Statement>(s);
}

std::any ModifyingVisitor::visit_stmt_variable_declaration(
    const std::shared_ptr<stmt::VariableDeclaration> &s)
{
    s->var_decl = std::dynamic_pointer_cast<decl::Variable>(
        result_or_nullptr<decl::Declaration>(visit(s->var_decl)));
    // Remove this variable declaration statement if the decl::Variable was removed
    return s->var_decl ? std::dynamic_pointer_cast<stmt::Statement>(s) : std::any();
}

std::any ModifyingVisitor::visit_stmt_expression(const std::shared_ptr<stmt::Expression> &s)
{
    s->expr = result_or_nullptr<expr::Expression>(visit(s->expr));
    // Remove this statement if the expression was removed
    return s->expr ? std::dynamic_pointer_cast<stmt::Statement>(s) : std::any();
}

std::any ModifyingVisitor::visit_expr_unary(const std::shared_ptr<expr::Unary> &e)
{
    e->expr = result_or_nullptr<expr::Expression>(visit(e->expr));
    // Remove this unary expression if the subexpression was removed
    return e->expr ? std::dynamic_pointer_cast<expr::Expression>(e) : std::any();
}

std::any ModifyingVisitor::visit_expr_binary(const std::shared_ptr<expr::Binary> &e)
{
    e->left = result_or_nullptr<expr::Expression>(visit(e->left));
    e->right = result_or_nullptr<expr::Expression>(visit(e->right));

    // Handle one of the subexpressions removing itself
    if (e->left && e->right) {
        return std::dynamic_pointer_cast<expr::Expression>(e);
    } else if (e->left) {
        return e->left;
    }
    return e->right;
}

std::any ModifyingVisitor::visit_expr_variable(const std::shared_ptr<expr::Variable> &e)
{
    return std::dynamic_pointer_cast<expr::Expression>(e);
}

std::any ModifyingVisitor::visit_expr_constant(const std::shared_ptr<expr::Constant> &e)
{
    return std::dynamic_pointer_cast<expr::Expression>(e);
}

std::any ModifyingVisitor::visit_expr_function_call(
    const std::shared_ptr<expr::FunctionCall> &e)
{
    std::vector<std::shared_ptr<expr::Expression>> args;
    for (auto &a : e->args) {
        collect_results(visit(a), args);
    }

    e->args = args;
    // Note: can't really visit the sub "fragments" of the struct/array access separately
    return std::dynamic_pointer_cast<expr::Expression>(e);
}

std::any ModifyingVisitor::visit_struct_array_access(
    const std::shared_ptr<expr::StructArrayAccess> &e)
{
    // Note: can't really visit the sub "fragments" of the struct/array access separately
    return std::dynamic_pointer_cast<expr::Expression>(e);
}

std::any ModifyingVisitor::visit_expr_assignment(const std::shared_ptr<expr::Assignment> &e)
{
    e->lhs = result_or_nullptr<expr::Expression>(visit(e->lhs));
    e->value = result_or_nullptr<expr::Expression>(visit(e->value));

    // If either the left hand side of the assignment or the value being assigned was removed,
    // we don't have an assignment expression anymore so the whole thing is dropped.
    if (e->lhs && e->value) {
        return std::dynamic_pointer_cast<expr::Expression>(e);
    }
    return std::any();
}
}
}
