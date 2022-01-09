#include "resolver_visitor.h"
#include "ast/visitor.h"
#include "error_listener.h"
#include "parallel_hashmap/phmap.h"

namespace crtl {

ResolverVisitor::SymbolStatus::SymbolStatus(ast::decl::Declaration *decl) : decl(decl) {}

std::any ResolverVisitor::visit_decl_function(ast::decl::Function *d)
{
    declare(d);
    define(d);

    begin_scope();
    // decl::Function's children are the parameters followed by the block,
    // so we can just visit them in order
    visit_children(d);
    end_scope();
    return std::any();
}
std::any ResolverVisitor::visit_decl_entry_point(ast::decl::EntryPoint *d)
{
    // Entry points are not callable from regular shader code, so we don't declare/define
    // them for resolution. Just push on a scope for the parameters and visit the node's
    // children (parameters and block)
    begin_scope();
    visit_children(d);
    end_scope();
    return std::any();
}

std::any ResolverVisitor::visit_decl_global_param(ast::decl::GlobalParam *d)
{
    // GlobalParam's have no initializer expression, no children to visit
    declare(d);
    define(d);
    return std::any();
}

std::any ResolverVisitor::visit_decl_struct(ast::decl::Struct *d)
{
    // No need to visit decl::Struct children, as members aren't resolved independently
    // of the struct itself
    declare(d);
    define(d);
    return std::any();
}

std::any ResolverVisitor::visit_decl_variable(ast::decl::Variable *d)
{
    // We define the variable after visiting its initializer expression to catch
    // errors where a variable is used in its own initializer
    declare(d);
    visit_children(d);
    define(d);
    return std::any();
}

std::any ResolverVisitor::visit_stmt_block(ast::stmt::Block *s)
{
    begin_scope();
    visit_children(s);
    end_scope();
    return std::any();
}

std::any ResolverVisitor::visit_stmt_for(ast::stmt::For *s)
{
    // Push on a scope for the for loop's loop variable declaration
    begin_scope();
    visit_children(s);
    end_scope();
    return std::any();
}

std::any ResolverVisitor::visit_expr_variable(ast::expr::Variable *e)
{
    // Resolve the variable referenced by the expression
    auto *var_decl = resolve_variable(e);
    if (var_decl) {
        resolved->var_expr[e] = var_decl;
    } else {
        report_error(e->get_token(), "Use of undeclared variable '" + e->name() + "'");
    }
    return std::any();
}

std::any ResolverVisitor::visit_expr_function_call(ast::expr::FunctionCall *e)
{
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
            auto *token = v.second.decl->get_token();
            report_warning(token, "Unused variable '" + token->getText() + "'");
        }
    }
    scopes.pop_back();
}

void ResolverVisitor::declare(ast::decl::Declaration *decl)
{
    auto *current_scope = scopes.empty() ? &global_scope : &scopes.back();
    auto *token = decl->get_token();
    auto fnd = current_scope->find(token->getText());
    if (fnd != current_scope->end()) {
        report_error(token, "Variable '" + token->getText() + "' has already been declared");
        return;
    }
    (*current_scope)[token->getText()] = SymbolStatus(decl);
}

void ResolverVisitor::define(ast::decl::Declaration *decl)
{
    auto *current_scope = scopes.empty() ? &global_scope : &scopes.back();
    (*current_scope)[decl->get_token()->getText()].defined = true;
}

ast::decl::Variable *ResolverVisitor::resolve_variable(ast::expr::Variable *node)
{
    const std::string ident = node->name();
    for (int i = scopes.size() - 1; i >= 0; --i) {
        auto &scope = scopes[i];
        auto fnd = scope.find(ident);
        if (fnd != scope.end()) {
            auto *var_decl = dynamic_cast<ast::decl::Variable *>(fnd->second.decl);
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
        auto *var_decl = dynamic_cast<ast::decl::Variable *>(fnd->second.decl);
        if (var_decl) {
            fnd->second.read = true;
            resolved->var_expr[node] = var_decl;
            return var_decl;
        }
    }
    return nullptr;
}

ast::decl::Struct *ResolverVisitor::resolve_struct(ast::ty::Struct *node)
{
    // Structs are only declared in global scope, so we only look there
    auto fnd = global_scope.find(node->name);
    if (fnd != global_scope.end()) {
        // Check that what we found is a struct declaration
        auto *decl = dynamic_cast<ast::decl::Struct *>(fnd->second.decl);
        if (decl) {
            fnd->second.read = true;
            resolved->struct_type[node] = decl;
            return decl;
        }
    }
    return nullptr;
}

ast::decl::Function *ResolverVisitor::resolve_function(ast::expr::FunctionCall *node)
{
    // Functions are only declared in global scope, so we only look there
    auto fnd = global_scope.find(node->get_token()->getText());
    if (fnd != global_scope.end()) {
        // Check that what we found is a struct declaration
        auto *decl = dynamic_cast<ast::decl::Function *>(fnd->second.decl);
        if (decl) {
            fnd->second.read = true;
            resolved->call_expr[node] = decl;
            return decl;
        }
    }
    return nullptr;
}
}
