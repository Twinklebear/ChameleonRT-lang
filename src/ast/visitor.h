#pragma once

#include <any>
#include <memory.h>
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

    virtual std::any visit_ast(const std::shared_ptr<AST> &ast);

    std::any visit(const std::shared_ptr<Node> &n);

    // Visit the node's children and return a vector<any> of their return values
    std::any visit_children(const std::shared_ptr<Node> &n);

    // Declarations
    virtual std::any visit_decl_function(const std::shared_ptr<decl::Function> &d);
    virtual std::any visit_decl_entry_point(const std::shared_ptr<decl::EntryPoint> &d);
    virtual std::any visit_decl_global_param(const std::shared_ptr<decl::GlobalParam> &d);
    virtual std::any visit_decl_struct(const std::shared_ptr<decl::Struct> &d);
    virtual std::any visit_decl_struct_member(const std::shared_ptr<decl::StructMember> &d);
    virtual std::any visit_decl_variable(const std::shared_ptr<decl::Variable> &d);

    // Statements
    virtual std::any visit_stmt_block(const std::shared_ptr<stmt::Block> &s);
    virtual std::any visit_stmt_if_else(const std::shared_ptr<stmt::IfElse> &s);
    virtual std::any visit_stmt_while(const std::shared_ptr<stmt::While> &s);
    virtual std::any visit_stmt_for(const std::shared_ptr<stmt::For> &s);
    virtual std::any visit_stmt_return(const std::shared_ptr<stmt::Return> &s);
    virtual std::any visit_stmt_variable_declaration(
        const std::shared_ptr<stmt::VariableDeclaration> &s);
    virtual std::any visit_stmt_expression(const std::shared_ptr<stmt::Expression> &s);

    // Expressions
    virtual std::any visit_expr_unary(const std::shared_ptr<expr::Unary> &e);
    virtual std::any visit_expr_binary(const std::shared_ptr<expr::Binary> &e);
    virtual std::any visit_expr_variable(const std::shared_ptr<expr::Variable> &e);
    virtual std::any visit_expr_constant(const std::shared_ptr<expr::Constant> &e);
    virtual std::any visit_expr_function_call(const std::shared_ptr<expr::FunctionCall> &e);
    virtual std::any visit_struct_array_access(
        const std::shared_ptr<expr::StructArrayAccess> &e);
    virtual std::any visit_expr_assignment(const std::shared_ptr<expr::Assignment> &e);

private:
    void report_enter_node(const Node *n);
    void report_exit_node(const Node *n);
};
}
}
