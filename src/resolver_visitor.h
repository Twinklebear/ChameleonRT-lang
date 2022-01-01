#pragma once

#include "ast/visitor.h"
#include "error_listener.h"
#include "parallel_hashmap/phmap.h"

namespace crtl {

/* The result of the resolver pass is that we have a mapping from:
 * - every struct type used to the declaration of the struct
 * - every variable expression to the declaration of the variable
 * - every function call expression to the declaration of the function
 */
struct ResolverPassResult {
    // A map of all struct type usages found in the code to the declaration for the struct
    phmap::parallel_flat_hash_map<ast::ty::Struct *, ast::decl::Struct *> struct_type;

    // A map of each variable expression found in the program to the declaration of that
    // variable
    phmap::parallel_flat_hash_map<ast::expr::Variable *, ast::decl::Variable *> var_expr;

    // A map of each function call expression in the program to the function being called
    phmap::parallel_flat_hash_map<ast::expr::FunctionCall *, ast::decl::Function *> call_expr;
};

class ResolverVisitor : public ast::Visitor, public crtl::ErrorReporter {
    struct VariableStatus {
        bool defined = false;
        bool read = false;

        ast::decl::Variable *var_decl = nullptr;

        VariableStatus() = default;

        VariableStatus(ast::decl::Variable *var_decl);
    };

    /* Track variables declared in each scope and whether they've been read or not, to report
     * warnings for unused variables This is treated as a stack, but we need to access scopes
     * by index as well when resolving variables
     */
    std::vector<phmap::parallel_flat_hash_map<std::string, VariableStatus>> scopes;

    // Map of all user defined structs so we can look them up by name
    phmap::parallel_flat_hash_map<std::string, ast::decl::Struct *> structs;

    // Map of all user defined functions so we can look them up by name
    phmap::parallel_flat_hash_map<std::string, ast::decl::Function *> functions;

public:
    std::shared_ptr<ResolverPassResult> resolved = std::make_shared<ResolverPassResult>();

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
    void begin_scope();

    void end_scope();

    void declare(const ast::decl::Variable *var);

    void define(const ast::decl::Variable *var);

    /* Find the descriptor for the struct with the given name.
     * Returns null and reports an error if there's no such struct
     */
    ast::decl::Struct *resolve_struct(const ast::ty::Struct *node);

    /* Resolve the variable referenced by the token and associate it with the
     * node.
     * Returns null and reports an error if there's no such struct
     */
    ast::decl::Variable *resolve_variable(const ast::expr::Variable *node);

    /* Resolve the function referenced by the call and associate it with the
     * node.
     * Returns null and reports an error if there's no such struct
     */
    ast::decl::Function *resolve_function(const ast::expr::FunctionCall *node);
};
}
