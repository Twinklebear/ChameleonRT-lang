#pragma once

#include "ast/visitor.h"
#include "resolver_visitor.h"

namespace crtl {

/* The RenameEntryPointParamVisitor renames all entry point parameters to be prefixed by the
 * entry point function name to avoid name collisions when they are moved in to the global
 * scope for HLSL and GLSL targets
 * TODO: Just prefixing by the function name may not be quite enough to ensure we avoid
 * collisions, if that's the case can generate more unique names and/or do some checking to
 * make sure we don't make a collision. For my own use this simple approach is fine
 */
class RenameEntryPointParamVisitor : public ast::Visitor {
    std::shared_ptr<ResolverPassResult> resolver_result;

    phmap::parallel_flat_hash_set<ast::decl::Variable *> renamed_vars;

public:
    RenameEntryPointParamVisitor(const std::shared_ptr<ResolverPassResult> &resolver_result);

    RenameEntryPointParamVisitor() = default;

    std::any visit_decl_entry_point(ast::decl::EntryPoint *d) override;

    std::any visit_expr_variable(ast::expr::Variable *e) override;
};

}