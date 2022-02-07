#include "global_struct_param_expansion_visitor.h"

namespace crtl {
using namespace ast;

GlobalStructParamExpansionVisitor::GlobalStructParamExpansionVisitor(
    const std::shared_ptr<ResolverPassResult> &resolver_result)
    : resolver_result(resolver_result)
{
}

std::any GlobalStructParamExpansionVisitor::visit_ast(const std::shared_ptr<ast::AST> &ast)
{
    // Make a copy of the AST to ensure any global param decl references remain valid as we
    // create the new tree with them expanded
    return std::any();
}

std::any GlobalStructParamExpansionVisitor::visit_decl_global_param(
    const std::shared_ptr<ast::decl::GlobalParam> &d)
{
    return std::any();
}

std::any GlobalStructParamExpansionVisitor::visit_expr_variable(
    const std::shared_ptr<ast::expr::Variable> &e)
{
    return std::any();
}

std::any GlobalStructParamExpansionVisitor::visit_struct_array_access(
    const std::shared_ptr<ast::expr::StructArrayAccess> &e)
{
    // TODO: Here we actually need to replace this node w/ a plain expr::Variable node
    // also need to make sure it's resolved to the new expanded decl in the resolver result for
    // later passes that need this info
    // Would writing this pass be much easier if the visitor's took std::shared_ptr<T>& instead
    // of raw pointers? then I could write visitors for statements and expressions that didn't
    // need to care if their child node was changed or not, it would just re-assign the thing
    // it got returned when visiting its sub-expression
    return std::any();
}
}
