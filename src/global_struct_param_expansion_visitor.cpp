#include "global_struct_param_expansion_visitor.h"

namespace crtl {
using namespace ast;

GlobalStructParamExpansionVisitor::GlobalStructParamExpansionVisitor(
    const std::shared_ptr<ResolverPassResult> &resolver_result)
    : resolver_result(resolver_result)
{
}

std::any GlobalStructParamExpansionVisitor::visit_decl_global_param(
    const std::shared_ptr<ast::decl::GlobalParam> &d)
{
    const auto struct_type = std::dynamic_pointer_cast<ast::ty::Struct>(d->get_type());
    if (!struct_type) {
        return std::dynamic_pointer_cast<decl::Declaration>(d);
    }

    // TODO: The decl needs to be expanded into individual global param decls for its members
    const auto struct_decl = resolver_result->struct_type[struct_type];
    std::vector<std::shared_ptr<decl::Declaration>> expanded_decls;
    auto expanded_param = std::make_shared<ExpandedGlobalParam>();
    for (const auto &m : struct_decl->members) {
        auto gp = std::make_shared<decl::GlobalParam>(
            d->get_text() + "_" + m->get_text(), nullptr, m->get_type());
        expanded_decls.push_back(gp);
        expanded_param->members[m->get_text()] = gp;
    }
    expanded_global_params[d] = expanded_param;
    return expanded_decls;
}

std::any GlobalStructParamExpansionVisitor::visit_expr_variable(
    const std::shared_ptr<ast::expr::Variable> &e)
{
    // Check if this variable expression is referencing an expanded global struct parameter
    auto resolved_decl = resolver_result->var_expr[e];
    auto global_decl = std::dynamic_pointer_cast<decl::GlobalParam>(resolved_decl);
    if (!global_decl || !expanded_global_params.contains(global_decl)) {
        return std::dynamic_pointer_cast<expr::Expression>(e);
    }

    // TODO: If we encounter an expanded global parameter struct being used directly (not
    // accessing its members), we have a problem as the struct no longer exists. This would
    // mean that we would actually need to go rewrite the code using the struct. This would
    // most likely be the case if the code was passing the global struct param to a function in
    // some library header. Right now this kind of use case isn't supported.
    // One option here might be to create the struct and set it up like is done for HLSL raygen
    // parameters so that we can pass it to the function? It's difficult to do that at this
    // point in the tree though, we would need to do this in the block containing this
    // expression so we can insert new statements
    report_error(e->get_token(),
                 "TODO: Direct use of expanded global struct parameter is not supported");
    return std::dynamic_pointer_cast<expr::Expression>(e);
}

std::any GlobalStructParamExpansionVisitor::visit_struct_array_access(
    const std::shared_ptr<ast::expr::StructArrayAccess> &e)
{
    // Check if this variable expression is referencing an expanded global struct parameter
    auto resolved_decl = resolver_result->var_expr[e->variable];
    auto global_decl = std::dynamic_pointer_cast<decl::GlobalParam>(resolved_decl);
    if (!global_decl || !expanded_global_params.contains(global_decl)) {
        return std::dynamic_pointer_cast<expr::Expression>(e);
    }

    // If this is a struct/array access on an expanded global struct parameter we replace the
    // struct array access expression with either a plain variable expression (if there's only
    // one struct_array_access fragment), or replace the variable in the expression with the
    // expanded one to replace the first struct_array_access fragment, which corresponds to the
    // original struct. The new variable expression is then resolved to the expanded global
    // parameter for subsequent passes.

    // We expect this must be a struct fragment, as an array fragment would not be valid here
    auto struct_fragment =
        std::dynamic_pointer_cast<expr::StructMemberAccessFragment>(e->struct_array_access[0]);
    if (!struct_fragment) {
        report_error(e->get_token(),
                     "Invalid direct array expression on expanded global struct parameter?");
        return std::dynamic_pointer_cast<expr::Expression>(e);
    }
    // Pop the struct member that's been made into its own standalone variable
    e->struct_array_access.erase(e->struct_array_access.begin());

    auto expanded_member =
        expanded_global_params[global_decl]->members[struct_fragment->name()];

    auto var_expr = std::make_shared<expr::Variable>(expanded_member->get_text());

    // Update the resolver data for this new variable expression
    resolver_result->var_expr[var_expr] = expanded_member;
    // Remove the old variable expression from the resolver data
    resolver_result->var_expr.erase(e->variable);

    // If there's just one struct_array_access fragment we replace the expression with a plain
    // variable expression
    if (e->struct_array_access.empty()) {
        return std::dynamic_pointer_cast<expr::Expression>(var_expr);
    }

    // Otherwise replace the variable part of the struct array access expression with the
    // expanded global
    e->variable = var_expr;
    return std::dynamic_pointer_cast<expr::Expression>(e);
}
}
