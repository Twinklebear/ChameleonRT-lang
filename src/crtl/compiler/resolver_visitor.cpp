#include "resolver_visitor.h"
#include "ast/visitor.h"
#include "error_listener.h"
#include "parallel_hashmap/phmap.h"

namespace crtl {

ResolverVisitor::SymbolStatus::SymbolStatus(
    const std::shared_ptr<ast::decl::Declaration> &decl)
    : decl(decl)
{
}

ResolverVisitor::ResolverVisitor(
    const std::vector<std::shared_ptr<ast::decl::Declaration>> &builtins)
{
    for (const auto &d : builtins) {
        declare(d);
        define(d);
    }
}

std::any ResolverVisitor::visit_decl_function(const std::shared_ptr<ast::decl::Function> &d)
{
    declare(d);
    define(d);

    begin_scope();
    // decl::Function's children are the parameters followed by the block,
    // so we can just visit them in order
    visit_children(d);
    end_scope();

    // Resolve the function's parameter and return types in the functions ty::Function
    // the parameter struct types (if any) will also be resolved when we visit the
    // decl::Function's children and visit each parameter's decl::Variable
    auto fn_ty = std::dynamic_pointer_cast<ast::ty::Function>(d->get_type());
    for (const auto &p : fn_ty->parameters) {
        // We don't need to report an error here because any errors with the parameter types
        // will have already been reported when we visited the parameter node
        resolve_type(p);
    }
    if (!resolve_type(fn_ty->return_type)) {
        report_error(d->get_token(),
                     "Use of undeclared struct '" + fn_ty->return_type->to_string() +
                         "' as return type");
    }
    return std::any();
}
std::any ResolverVisitor::visit_decl_entry_point(
    const std::shared_ptr<ast::decl::EntryPoint> &d)
{
    // Entry points are not callable from regular shader code, so we don't declare/define
    // them for resolution. Just push on a scope for the parameters and visit the node's
    // children (parameters and block)
    begin_scope();
    visit_children(d);
    end_scope();
    return std::any();
}

std::any ResolverVisitor::visit_decl_global_param(
    const std::shared_ptr<ast::decl::GlobalParam> &d)
{
    // GlobalParam's have no initializer expression, so no children to visit. We just need to
    // resolve its type and declare/define it
    if (!resolve_type(d->get_type())) {
        report_error(d->get_token(),
                     "Use of undeclared struct '" + d->get_type()->to_string() + "'");
        return std::any();
    }
    declare(d);
    define(d);
    return std::any();
}

std::any ResolverVisitor::visit_decl_struct(const std::shared_ptr<ast::decl::Struct> &d)
{
    // No need to visit decl::Struct children, as members aren't resolved independently
    // of the struct itself
    declare(d);
    define(d);
    return std::any();
}

std::any ResolverVisitor::visit_decl_variable(const std::shared_ptr<ast::decl::Variable> &d)
{
    if (!resolve_type(d->get_type())) {
        report_error(d->get_token(),
                     "Use of undeclared struct '" + d->get_type()->to_string() + "'");
        return std::any();
    }
    // We define the variable after visiting its initializer expression to catch
    // errors where a variable is used in its own initializer
    declare(d);
    visit_children(d);
    define(d);
    return std::any();
}

std::any ResolverVisitor::visit_stmt_block(const std::shared_ptr<ast::stmt::Block> &s)
{
    begin_scope();
    visit_children(s);
    end_scope();
    return std::any();
}

std::any ResolverVisitor::visit_stmt_for(const std::shared_ptr<ast::stmt::For> &s)
{
    // Push on a scope for the for loop's loop variable declaration
    begin_scope();
    visit_children(s);
    end_scope();
    return std::any();
}

std::any ResolverVisitor::visit_expr_variable(const std::shared_ptr<ast::expr::Variable> &e)
{
    // Resolve the variable referenced by the expression
    auto var_decl = resolve_variable(e);
    if (var_decl) {
        resolved->var_expr[e] = var_decl;
    } else {
        report_error(e->get_token(), "Use of undeclared variable '" + e->name() + "'");
    }
    return std::any();
}

std::any ResolverVisitor::visit_expr_function_call(
    const std::shared_ptr<ast::expr::FunctionCall> &e)
{
    auto fn_decl = resolve_function(e);
    if (fn_decl) {
        resolved->call_expr[e] = fn_decl;
    } else {
        report_error(e->get_token(), "Call of undeclared function '" + e->get_text() + "'");
    }
    return std::any();
}

void ResolverVisitor::begin_scope()
{
    scopes.emplace_back();
}

void ResolverVisitor::end_scope()
{
    for (auto &v : scopes.back()) {
        if (!v.second.read) {
            auto decl = v.second.decl;
            report_warning(decl->get_token(), "Unused variable '" + decl->get_text() + "'");
        }
    }
    scopes.pop_back();
}

void ResolverVisitor::declare(const std::shared_ptr<ast::decl::Declaration> &decl)
{
    auto *current_scope = scopes.empty() ? &global_scope : &scopes.back();
    const std::string decl_name = decl->get_text();
    auto fnd = current_scope->find(decl_name);
    if (fnd != current_scope->end()) {
        report_error(decl->get_token(),
                     "Variable '" + decl_name + "' has already been declared");
        return;
    }
    (*current_scope)[decl_name] = SymbolStatus(decl);
}

void ResolverVisitor::define(const std::shared_ptr<ast::decl::Declaration> &decl)
{
    auto *current_scope = scopes.empty() ? &global_scope : &scopes.back();
    (*current_scope)[decl->get_text()].defined = true;
}

bool ResolverVisitor::resolve_type(const std::shared_ptr<ast::ty::Type> &type)
{
    if (type->base_type != ast::ty::BaseType::STRUCT) {
        return true;
    }
    auto struct_type = std::dynamic_pointer_cast<ast::ty::Struct>(type);
    auto struct_decl = resolve_struct(struct_type);
    if (struct_decl) {
        resolved->struct_type[struct_type] = struct_decl;
    }
    return struct_decl != nullptr;
}

std::shared_ptr<ast::decl::Variable> ResolverVisitor::resolve_variable(
    const std::shared_ptr<ast::expr::Variable> &node)
{
    const std::string ident = node->name();
    for (int i = scopes.size() - 1; i >= 0; --i) {
        auto &scope = scopes[i];
        auto fnd = scope.find(ident);
        if (fnd != scope.end()) {
            auto var_decl = std::dynamic_pointer_cast<ast::decl::Variable>(fnd->second.decl);
            // Every decl in a local scope should be a variable, as struct/function/globalparam
            // decls are not allowed at the parser level
            if (!var_decl) {
                return nullptr;
            }
            if (!fnd->second.defined) {
                report_error(node->get_token(), "Cannot read variable in its own initializer");
                return nullptr;
            } else {
                fnd->second.read = true;
                resolved->var_expr[node] = var_decl;
                return var_decl;
            }
        }
    }

    // If we didn't find it in a local scope check the global scope for a global variable/param
    // being accessed
    auto fnd = global_scope.find(ident);
    if (fnd != global_scope.end()) {
        auto var_decl = std::dynamic_pointer_cast<ast::decl::Variable>(fnd->second.decl);
        if (var_decl) {
            fnd->second.read = true;
            resolved->var_expr[node] = var_decl;
            return var_decl;
        }
    }
    return nullptr;
}

std::shared_ptr<ast::decl::Struct> ResolverVisitor::resolve_struct(
    const std::shared_ptr<ast::ty::Struct> &node)
{
    // Structs are only declared in global scope, so we only look there
    auto fnd = global_scope.find(node->name);
    if (fnd != global_scope.end()) {
        // Check that what we found is a struct declaration
        auto decl = std::dynamic_pointer_cast<ast::decl::Struct>(fnd->second.decl);
        if (decl) {
            fnd->second.read = true;
            resolved->struct_type[node] = decl;
            return decl;
        }
    }
    return nullptr;
}

std::shared_ptr<ast::decl::Function> ResolverVisitor::resolve_function(
    const std::shared_ptr<ast::expr::FunctionCall> &node)
{
    // Functions are only declared in global scope, so we only look there
    auto fnd = global_scope.find(node->get_text());
    if (fnd != global_scope.end()) {
        // Check that what we found is a struct declaration
        auto decl = std::dynamic_pointer_cast<ast::decl::Function>(fnd->second.decl);
        if (decl) {
            fnd->second.read = true;
            resolved->call_expr[node] = decl;
            return decl;
        }
    }
    return nullptr;
}
}
