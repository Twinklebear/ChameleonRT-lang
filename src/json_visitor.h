#pragma once

#include "ast/visitor.h"
#include "json.hpp"

namespace crtl {

class JSONVisitor : public ast::Visitor {
public:
    /* Visit the AST to output it to a JSON representation. Returns the nlohmann::json object
     * containing the JSON representation of the AST
     */
    std::any visit_ast(const std::shared_ptr<ast::AST> &ast) override;

    std::any visit_decl_function(const std::shared_ptr<ast::decl::Function> &d) override;
    std::any visit_decl_entry_point(const std::shared_ptr<ast::decl::EntryPoint> &d) override;
    std::any visit_decl_global_param(
        const std::shared_ptr<ast::decl::GlobalParam> &d) override;
    std::any visit_decl_struct(const std::shared_ptr<ast::decl::Struct> &d) override;
    std::any visit_decl_struct_member(
        const std::shared_ptr<ast::decl::StructMember> &d) override;
    std::any visit_decl_variable(const std::shared_ptr<ast::decl::Variable> &d) override;

    std::any visit_stmt_block(const std::shared_ptr<ast::stmt::Block> &s) override;
    std::any visit_stmt_if_else(const std::shared_ptr<ast::stmt::IfElse> &s) override;
    std::any visit_stmt_while(const std::shared_ptr<ast::stmt::While> &s) override;
    std::any visit_stmt_for(const std::shared_ptr<ast::stmt::For> &s) override;
    std::any visit_stmt_return(const std::shared_ptr<ast::stmt::Return> &s) override;
    std::any visit_stmt_variable_declaration(
        const std::shared_ptr<ast::stmt::VariableDeclaration> &s) override;
    std::any visit_stmt_expression(const std::shared_ptr<ast::stmt::Expression> &s) override;

    std::any visit_expr_unary(const std::shared_ptr<ast::expr::Unary> &e) override;
    std::any visit_expr_binary(const std::shared_ptr<ast::expr::Binary> &e) override;
    std::any visit_expr_variable(const std::shared_ptr<ast::expr::Variable> &e) override;
    std::any visit_expr_constant(const std::shared_ptr<ast::expr::Constant> &e) override;
    std::any visit_expr_function_call(
        const std::shared_ptr<ast::expr::FunctionCall> &e) override;
    std::any visit_struct_array_access(
        const std::shared_ptr<ast::expr::StructArrayAccess> &e) override;
    std::any visit_expr_assignment(const std::shared_ptr<ast::expr::Assignment> &e) override;

private:
    // Visit all the nodes in the vector and return their results as a vector of JSON elements
    std::vector<nlohmann::json> visit_all(
        const std::vector<std::shared_ptr<ast::Node>> &nodes);

    // Visit all the struct/array access fragments and return them as an vector of JSON
    // elements
    std::vector<nlohmann::json> visit_struct_array_fragments(
        const std::vector<std::shared_ptr<ast::expr::StructArrayAccessFragment>> &fragments);
};

}
