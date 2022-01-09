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
    // variable. Each variable expr may be accessing a local/global variable decl or a global
    // parameter
    phmap::parallel_flat_hash_map<ast::expr::Variable *, ast::decl::Variable *> var_expr;

    // A map of each function call expression in the program to the function being called
    phmap::parallel_flat_hash_map<ast::expr::FunctionCall *, ast::decl::Function *> call_expr;
};

class ResolverVisitor : public ast::Visitor, public crtl::ErrorReporter {
    // The status of a variable, struct, or function defined in the program.
    struct SymbolStatus {
        bool defined = false;
        bool read = false;

        ast::decl::Declaration *decl = nullptr;

        SymbolStatus() = default;

        SymbolStatus(ast::decl::Declaration *decl);
    };

    // Map of user defined global variables, parameters, structs, functions, entry points
    phmap::parallel_flat_hash_map<std::string, SymbolStatus> global_scope;

    /* Track variables declared in each scope and whether they've been read or not, to report
     * warnings for unused variables This is treated as a stack, but we need to access scopes
     * by index as well when resolving variables
     */
    std::vector<phmap::parallel_flat_hash_map<std::string, SymbolStatus>> scopes;

public:
    std::shared_ptr<ResolverPassResult> resolved = std::make_shared<ResolverPassResult>();

    std::any visit_decl_function(ast::decl::Function *d) override;
    std::any visit_decl_entry_point(ast::decl::EntryPoint *d) override;
    std::any visit_decl_global_param(ast::decl::GlobalParam *d) override;
    std::any visit_decl_struct(ast::decl::Struct *d) override;
    std::any visit_decl_variable(ast::decl::Variable *d) override;

    std::any visit_stmt_block(ast::stmt::Block *s) override;
    std::any visit_stmt_for(ast::stmt::For *s) override;

    std::any visit_expr_variable(ast::expr::Variable *e) override;
    std::any visit_expr_function_call(ast::expr::FunctionCall *e) override;

private:
    void begin_scope();

    void end_scope();

    void declare(ast::decl::Declaration *decl);

    void define(ast::decl::Declaration *decl);

    /* Resolve the struct type to the corresponding struct declaration, if the type passed is a
     * struct. Returns true if the struct declaration was resolved or the type is not a struct
     * (and thus requires no resolution). Returns false if the struct declaration was not found
     */
    bool resolve_type(const std::shared_ptr<ast::ty::Type> &type);

    /* Resolve the variable referenced by the token and associate it with the
     * node.
     * Returns null and reports an error if there's no such struct
     */
    ast::decl::Variable *resolve_variable(ast::expr::Variable *node);

    /* Find the descriptor for the struct with the given name.
     * Returns null and reports an error if there's no such struct
     */
    ast::decl::Struct *resolve_struct(ast::ty::Struct *node);

    /* Resolve the function referenced by the call and associate it with the
     * node.
     * Returns null and reports an error if there's no such struct
     */
    ast::decl::Function *resolve_function(ast::expr::FunctionCall *node);
};
}
