#pragma once

#include "ast/visitor.h"
#include "resolver_visitor.h"

namespace crtl {
namespace hlsl {

// TODO: Later can have a parent for all langage output visitors that will hold the generated
// source, metadata, etc.
class OutputVisitor : public ast::Visitor {
    std::shared_ptr<ResolverPassResult> resolver_result;

    int current_register_space = 0;
    int next_srv_slot = 0;
    int next_sampler_slot = 0;
    int next_uav_slot = 0;
    int next_cbv_slot = 0;

public:
    OutputVisitor(const std::shared_ptr<ResolverPassResult> &resolver_result);

    OutputVisitor() = default;

    // NOTE: Most statements don't need any rewriting but we do still need to visit
    // everything to build the HLSL source code

    /* Visit the AST and translate it to HLSL. The std::any returned contains a std::string
     * with the translated source code
     */
    std::any visit_ast(ast::AST *ast) override;

    // Declarations
    std::any visit_decl_function(ast::decl::Function *d) override;
    std::any visit_decl_entry_point(ast::decl::EntryPoint *d) override;
    std::any visit_decl_global_param(ast::decl::GlobalParam *d) override;
    std::any visit_decl_struct(ast::decl::Struct *d) override;
    std::any visit_decl_struct_member(ast::decl::StructMember *d) override;
    std::any visit_decl_variable(ast::decl::Variable *d) override;

    // Statements
    std::any visit_stmt_block(ast::stmt::Block *s) override;
    std::any visit_stmt_if_else(ast::stmt::IfElse *s) override;
    std::any visit_stmt_while(ast::stmt::While *s) override;
    std::any visit_stmt_for(ast::stmt::For *s) override;
    std::any visit_stmt_return(ast::stmt::Return *s) override;
    std::any visit_stmt_variable_declaration(ast::stmt::VariableDeclaration *s) override;
    std::any visit_stmt_expression(ast::stmt::Expression *s) override;

    // Expressions
    std::any visit_expr_unary(ast::expr::Unary *e) override;
    std::any visit_expr_binary(ast::expr::Binary *e) override;
    std::any visit_expr_variable(ast::expr::Variable *e) override;
    std::any visit_expr_constant(ast::expr::Constant *e) override;
    std::any visit_expr_function_call(ast::expr::FunctionCall *e) override;
    std::any visit_struct_array_access(ast::expr::StructArrayAccess *e) override;
    std::any visit_expr_assignment(ast::expr::Assignment *e) override;
};

}
}

