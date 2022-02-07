#pragma once

#include <any>
#include <memory.h>
#include "visitor.h"

namespace crtl {
namespace ast {

/* The ModifyingVisitor's default visit methods will replace the child node being
 * visited with the result returned by visiting it, allowing nodes of the AST to be easily
 * replaced
 */
class ModifyingVisitor : public Visitor {
    virtual ~ModifyingVisitor() override = default;

    /* visit_ast can replace statements with new single nodes or with sets of statements.
     * Visiting a top-level declaration can return a single shared_ptr to a top level
     * declaration to replace the original node with or a vector<shared_ptr> to replace the
     * original node with a set of declarations
     */
    virtual std::any visit_ast(const std::shared_ptr<AST> &ast) override;

    // Declarations
    virtual std::any visit_decl_function(const std::shared_ptr<decl::Function> &d) override;
    virtual std::any visit_decl_entry_point(
        const std::shared_ptr<decl::EntryPoint> &d) override;
    virtual std::any visit_decl_global_param(
        const std::shared_ptr<decl::GlobalParam> &d) override;
    virtual std::any visit_decl_struct(const std::shared_ptr<decl::Struct> &d) override;
    virtual std::any visit_decl_struct_member(
        const std::shared_ptr<decl::StructMember> &d) override;
    virtual std::any visit_decl_variable(const std::shared_ptr<decl::Variable> &d) override;

    /* Statements.
     *
     * Similar to top level declarations, statements can be replaced with single statements
     * by returning a shared_ptr<stmt::Statement>, or with multiple statements by returning
     * a vector<shared_ptr<stmt::Statement>>
     * Visiting a top-level declaration can return a single shared_ptr to a top level statement
     * or declaration to replace the passed node with, or a vector<shared_ptr> to replace the
     * passed node with a set of statements
     */
    virtual std::any visit_stmt_block(const std::shared_ptr<stmt::Block> &s) override;
    virtual std::any visit_stmt_if_else(const std::shared_ptr<stmt::IfElse> &s) override;
    virtual std::any visit_stmt_while(const std::shared_ptr<stmt::While> &s) override;
    virtual std::any visit_stmt_for(const std::shared_ptr<stmt::For> &s) override;
    virtual std::any visit_stmt_return(const std::shared_ptr<stmt::Return> &s) override;
    virtual std::any visit_stmt_variable_declaration(
        const std::shared_ptr<stmt::VariableDeclaration> &s) override;
    virtual std::any visit_stmt_expression(
        const std::shared_ptr<stmt::Expression> &s) override;

    // Expressions
    virtual std::any visit_expr_unary(const std::shared_ptr<expr::Unary> &e) override;
    virtual std::any visit_expr_binary(const std::shared_ptr<expr::Binary> &e) override;
    virtual std::any visit_expr_variable(const std::shared_ptr<expr::Variable> &e) override;
    virtual std::any visit_expr_constant(const std::shared_ptr<expr::Constant> &e) override;
    virtual std::any visit_expr_function_call(
        const std::shared_ptr<expr::FunctionCall> &e) override;
    virtual std::any visit_struct_array_access(
        const std::shared_ptr<expr::StructArrayAccess> &e) override;
    virtual std::any visit_expr_assignment(
        const std::shared_ptr<expr::Assignment> &e) override;
};
}
}
