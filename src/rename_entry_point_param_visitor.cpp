#include "rename_entry_point_param_visitor.h"

namespace crtl {
using namespace ast;

RenameEntryPointParamVisitor::RenameEntryPointParamVisitor(
    const std::shared_ptr<ResolverPassResult> &resolver_result)
    : resolver_result(resolver_result)
{
}

std::any RenameEntryPointParamVisitor::visit_decl_entry_point(
    const std::shared_ptr<ast::decl::EntryPoint> &d)
{
    // Rename any parameters here
    const std::string prefix = d->get_text() + "_";
    for (auto &p : d->parameters) {
        auto &symbol = p->get_symbol();
        symbol->name = prefix + symbol->name;
        renamed_vars.insert(p);
    }

    // Visit the block of the entry point to rename any references
    visit(d->block);
    return std::any();
}

std::any RenameEntryPointParamVisitor::visit_expr_variable(
    const std::shared_ptr<ast::expr::Variable> &e)
{
    // If this was a variable that got renamed in this past, update its text in the expression
    auto decl = resolver_result->var_expr[e];
    if (renamed_vars.contains(decl)) {
        e->var_name = decl->get_text();
    }

    return std::any();
}
}
