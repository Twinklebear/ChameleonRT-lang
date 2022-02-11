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
    std::cout << "Expanding " << d->get_text() << " to\n";
    for (const auto &m : struct_decl->members) {
        std::cout << "\t" << d->get_text() + "_" + m->get_text()
                  << " : type = " << m->get_type()->to_string() << "\n";

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
    // TODO we need to update this variable if it referenced an expanded global param
    // TODO: Technically for a struct parameter that's been expanded, we should never encounter
    // such a statement. if we do it means the struct is being passed as a parameter to a
    // function in the code, which we don't support in this expansion thing. So I think this
    // should just error out
    return std::dynamic_pointer_cast<expr::Expression>(e);
}

std::any GlobalStructParamExpansionVisitor::visit_struct_array_access(
    const std::shared_ptr<ast::expr::StructArrayAccess> &e)
{
    // TODO we need to update this variable if it referenced an expanded global param
    // TODO: Here we actually need to replace this node w/ a plain expr::Variable node
    // also need to make sure it's resolved to the new expanded decl in the resolver result for
    // later passes that need this info
    return std::dynamic_pointer_cast<expr::Expression>(e);
}
}
