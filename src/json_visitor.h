#pragma once

#include "ast/visitor.h"
#include "json.hpp"

namespace crtl {

class JSONVisitor : public ast::Visitor {
public:
    nlohmann::json ast_json;

    std::any visit_ast(ast::AST *ast) override;

    std::any visit_decl_function(ast::decl::Function *d) override;
    std::any visit_decl_entry_point(ast::decl::EntryPoint *d) override;
    std::any visit_decl_global_param(ast::decl::GlobalParam *d) override;
    std::any visit_decl_struct(ast::decl::Struct *d) override;
    std::any visit_decl_struct_member(ast::decl::StructMember *d) override;
    std::any visit_decl_variable(ast::decl::Variable *d) override;

    std::any visit_stmt_block(ast::stmt::Block *s) override;
    std::any visit_stmt_if_else(ast::stmt::IfElse *s) override;
    std::any visit_stmt_while(ast::stmt::While *s) override;
    std::any visit_stmt_for(ast::stmt::For *s) override;
    std::any visit_stmt_return(ast::stmt::Return *s) override;
    std::any visit_stmt_variable_declaration(ast::stmt::VariableDeclaration *s) override;
    std::any visit_stmt_expression(ast::stmt::Expression *s) override;

    std::any visit_expr_unary(ast::expr::Unary *e) override;
    std::any visit_expr_binary(ast::expr::Binary *e) override;
    std::any visit_expr_variable(ast::expr::Variable *e) override;
    std::any visit_expr_constant(ast::expr::Constant *e) override;
    std::any visit_expr_function_call(ast::expr::FunctionCall *e) override;
    std::any visit_struct_array_access(ast::expr::StructArrayAccess *e) override;
    std::any visit_expr_assignment(ast::expr::Assignment *e) override;

private:
    // Visit all the nodes in the vector and return their results as a vector of JSON elements
    std::vector<nlohmann::json> visit_all(const std::vector<ast::Node *> &nodes);

    // Visit all the struct/array access fragments and return them as an vector of JSON
    // elements
    std::vector<nlohmann::json> visit_struct_array_fragments(
        const std::vector<std::shared_ptr<ast::expr::StructArrayAccessFragment>> &fragments);
};

}
