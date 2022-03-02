#pragma once

#include "ast/modifying_visitor.h"
#include "resolver_visitor.h"

namespace crtl {

struct ExpandedGlobalParam {
    // The former members of the global struct parameter that have been expanded to global
    // parameters
    phmap::parallel_flat_hash_map<std::string, std::shared_ptr<ast::decl::GlobalParam>>
        members;
};

// TODO: Need to not split up a struct of all constants into individual constant parameters
// Maybe this should become more backend-specific because what we want to split or not split
// depends on our compile target
class GlobalStructParamExpansionVisitor : public ast::ModifyingVisitor {
    std::shared_ptr<ResolverPassResult> resolver_result;

public:
    phmap::parallel_flat_hash_map<std::shared_ptr<ast::decl::GlobalParam>,
                                  std::shared_ptr<ExpandedGlobalParam>>
        expanded_global_params;

    GlobalStructParamExpansionVisitor(
        const std::shared_ptr<ResolverPassResult> &resolver_result);

    std::any visit_decl_global_param(
        const std::shared_ptr<ast::decl::GlobalParam> &d) override;

    /* We don't actually rewrite plain variable expressions, but need to visit them to check if
     * a global struct parameter was passed directly to a function, which the current design of
     * this pass turns into invalid code by replacing the struct param with its members
     */
    std::any visit_expr_variable(const std::shared_ptr<ast::expr::Variable> &e) override;

    std::any visit_struct_array_access(
        const std::shared_ptr<ast::expr::StructArrayAccess> &e) override;
};
}
