#pragma once

#include <any>
#include "declaration.h"
#include "error_listener.h"
#include "expression.h"
#include "node.h"
#include "statement.h"

namespace crtl {
namespace ast {

class Visitor : public ErrorReporter {
private:
    size_t trace_depth = 0;

public:
    bool trace_visitor = false;

    virtual ~Visitor() = default;

    virtual std::any visit_ast(AST *ast);

    std::any visit(Node *n);

    // Visit the node's children and return a vector<any> of their return values
    std::any visit_children(Node *n);

    // Declarations
    virtual std::any visit_decl_function(decl::Function *d);
    virtual std::any visit_decl_entry_point(decl::EntryPoint *d);
    virtual std::any visit_decl_global_param(decl::GlobalParam *d);
    virtual std::any visit_decl_struct(decl::Struct *d);
    virtual std::any visit_decl_struct_member(decl::StructMember *d);
    virtual std::any visit_decl_variable(decl::Variable *d);

    // Statements
    virtual std::any visit_stmt_block(stmt::Block *s);
    virtual std::any visit_stmt_if_else(stmt::IfElse *s);
    virtual std::any visit_stmt_while(stmt::While *s);
    virtual std::any visit_stmt_for(stmt::For *s);
    virtual std::any visit_stmt_return(stmt::Return *s);
    virtual std::any visit_stmt_variable_declaration(stmt::VariableDeclaration *s);
    virtual std::any visit_stmt_expression(stmt::Expression *s);

    // Expressions
    virtual std::any visit_expr_unary(expr::Unary *e);
    virtual std::any visit_expr_binary(expr::Binary *e);
    virtual std::any visit_expr_variable(expr::Variable *e);
    virtual std::any visit_expr_constant(expr::Constant *e);
    virtual std::any visit_expr_function_call(expr::FunctionCall *e);
    virtual std::any visit_struct_array_access(expr::StructArrayAccess *e);
    virtual std::any visit_expr_assignment(expr::Assignment *e);

private:
    void report_enter_node(const Node *n);
    void report_exit_node(const Node *n);
};
}
}
