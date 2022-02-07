#pragma once

#include "ast/visitor.h"
#include "resolver_visitor.h"

namespace crtl {

struct ExpandedGlobalParam {
    // The original struct type declaration that was expanded
    ast::decl::Struct *original_struct = nullptr;

    // The former members of the global struct parameter that have been expanded to global
    // parameters
    phmap::parallel_flat_hash_map<std::string, ast::decl::GlobalParam *> expanded_members;
};

class GlobalStructParamExpansionVisitor : public ast::Visitor {
    std::shared_ptr<ResolverPassResult> resolver_result;

    phmap::parallel_flat_hash_map<ast::decl::GlobalParam *,
                                  std::shared_ptr<ExpandedGlobalParam>>
        expanded_global_params;

public:
    GlobalStructParamExpansionVisitor(
        const std::shared_ptr<ResolverPassResult> &resolver_result);

    GlobalStructParamExpansionVisitor() = default;

    std::any visit_ast(ast::AST *ast) override;

    std::any visit_decl_global_param(ast::decl::GlobalParam *d) override;

    /* We don't actually rewrite plain variable expressions, but need to visit them to check if
     * a global struct parameter was passed directly to a function, which the current design of
     * this pass turns into invalid code by replacing the struct param with its members
     */
    std::any visit_expr_variable(ast::expr::Variable *e) override;

    std::any visit_struct_array_access(ast::expr::StructArrayAccess *e) override;
};

}
