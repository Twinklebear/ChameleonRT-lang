#include <memory>
#include "error_listener.h"
#include "resolver_visitor.h"
#include "struct_descriptor.h"

namespace crtl {

ResolverVisitor::VariableStatus::VariableStatus(
    const std::shared_ptr<VariableDescriptor> &var_desc)
    : var_desc(var_desc)
{
}

ResolverVisitor::ResolverVisitor(
    std::unordered_map<std::string, std::shared_ptr<StructDescriptor>> &in_structs)
    : structs(&in_structs)
{
    // Loop over the structs and resolve any member variable struct types
    for (auto &s : *structs) {
        for (auto &v : s.second->members) {
            if (v.type.type == VariableType::STRUCT) {
                v.type.struct_type = resolve_struct(v.type.node->IDENTIFIER()->getSymbol());
            }
        }
    }
}

antlrcpp::Any ResolverVisitor::visitVarDecl(crtg::ChameleonRTParser::VarDeclContext *ctx)
{
    auto type = TypeDescriptor(ctx->typeName());

    // Resolve the type if it's a user struct
    if (type.type == VariableType::STRUCT) {
        type.struct_type = resolve_struct(ctx->typeName()->IDENTIFIER()->getSymbol());
    }

    auto var_desc =
        std::make_shared<VariableDescriptor>(type, ctx->IDENTIFIER(), ctx->CONST() != nullptr);
    declare(ctx->IDENTIFIER()->getSymbol(), var_desc);

    // Visit any initializer for this variable
    visitChildren(ctx);

    define(ctx->IDENTIFIER()->getSymbol());
    return antlrcpp::Any();
}

antlrcpp::Any ResolverVisitor::visitBlock(crtg::ChameleonRTParser::BlockContext *ctx)
{
    begin_scope();
    visitChildren(ctx);
    end_scope();
    return antlrcpp::Any();
}

antlrcpp::Any ResolverVisitor::visitFunctionCall(
    crtg::ChameleonRTParser::FunctionCallContext *ctx)
{
    // Resolve the function being called, make sure it's actually a function
    auto callee = resolve_reference(ctx, ctx->IDENTIFIER()->getSymbol());
    if (!std::dynamic_pointer_cast<ShaderFunctionDescriptor>(callee)) {
        pass_error = true;
        report_error(
            ctx->IDENTIFIER()->getSymbol(),
            "Cannot call non-function '" + ctx->IDENTIFIER()->getSymbol()->getText() + "'");
    }

    // Now resolve the arguments to the function
    visitChildren(ctx);
    return antlrcpp::Any();
}

antlrcpp::Any ResolverVisitor::visitFunctionDecl(
    crtg::ChameleonRTParser::FunctionDeclContext *ctx)
{
    const std::string name = ctx->IDENTIFIER()->getText();
    std::shared_ptr<ShaderFunctionDescriptor> fcn;
    try {
        if (ctx->entryPointType()) {
            auto entry_pt = std::make_shared<ShaderEntryPointDescriptor>(ctx);
            declare(ctx->IDENTIFIER()->getSymbol(), entry_pt);
            fcn = entry_pt;
        } else {
            fcn = std::make_shared<ShaderFunctionDescriptor>(ctx);
            declare(ctx->IDENTIFIER()->getSymbol(), fcn);
        }
    } catch (const std::runtime_error &e) {
        report_error(ctx->IDENTIFIER()->getSymbol(), e.what());
        pass_error = true;
        return antlrcpp::Any();
    }

    // Push on a scope and define the function parameters
    begin_scope();
    for (const auto &p : fcn->parameters) {
        declare(p->ident->getSymbol(), p);
        define(p->ident->getSymbol());
    }

    // Descend into the function and resolve variable access and use within it
    visit(ctx->block());

    end_scope();

    return antlrcpp::Any();
}

antlrcpp::Any ResolverVisitor::visitGlobalParamDecl(
    crtg::ChameleonRTParser::GlobalParamDeclContext *ctx)
{
    const std::string param_name = ctx->IDENTIFIER()->getText();
    const TypeDescriptor type(ctx->typeName());
    if (type.type == VariableType::VOID) {
        pass_error = true;
        report_error(ctx->IDENTIFIER()->getSymbol(), "Global parameters cannot be void");
        return antlrcpp::Any();
    }
    declare(ctx->IDENTIFIER()->getSymbol(), std::make_shared<GlobalParameterDescriptor>(ctx));

    // Global initializers should only be literal values or references to
    // other globals, so nothing to do here. We could visit the children
    // and ensure that any initalizer does not contain a function call,
    // or some other invalid stuff to report this error here instead of in
    // the backend shader compiler
    return antlrcpp::Any();
}

antlrcpp::Any ResolverVisitor::visitPrimary(crtg::ChameleonRTParser::PrimaryContext *ctx)
{
    if (ctx->IDENTIFIER()) {
        resolve_reference(ctx, ctx->IDENTIFIER()->getSymbol());
    }
    return antlrcpp::Any();
}

antlrcpp::Any ResolverVisitor::visitStructArray(
    crtg::ChameleonRTParser::StructArrayContext *ctx)
{
    resolve_reference(ctx, ctx->IDENTIFIER()->getSymbol());
    visitChildren(ctx);
    return antlrcpp::Any();
}

antlrcpp::Any ResolverVisitor::visitAssign(crtg::ChameleonRTParser::AssignContext *ctx)
{
    resolve_reference(ctx, ctx->IDENTIFIER()->getSymbol());
    visitChildren(ctx);
    return antlrcpp::Any();
}

void ResolverVisitor::begin_scope()
{
    scopes.push_back(std::unordered_map<std::string, VariableStatus>());
}

void ResolverVisitor::end_scope()
{
    auto &current = scopes.back();
    for (const auto &v : current) {
        if (!v.second.read) {
            std::cerr << "Warning: local variable " << v.first << " declared at "
                      << v.second.var_desc->ident->getSymbol()->getLine() << ":"
                      << v.second.var_desc->ident->getSymbol()->getCharPositionInLine()
                      << " is never read\n";
        }
    }
    scopes.pop_back();
}

void ResolverVisitor::declare(const antlr4::Token *token,
                              const std::shared_ptr<VariableDescriptor> &var)
{
    if (scopes.empty()) {
        // All names of globals must be unique, so check for name collisions now
        {
            auto fnd = global_params.find(var->name);
            if (fnd != global_params.end()) {
                report_error(
                    token,
                    "A global variable with the name " + var->name + " already exists.");
                pass_error = true;
                return;
            }
        }
        {
            auto fnd = functions.find(var->name);
            if (fnd != functions.end()) {
                report_error(token,
                             "A function with the name " + var->name + " already exists.");
                pass_error = true;
                return;
            }
        }
        {
            auto fnd = functions.find(var->name);
            if (fnd != functions.end()) {
                report_error(token,
                             "An entry point with the name " + var->name + " already exists.");
                pass_error = true;
                return;
            }
        }
        {
            auto fnd = global_vars.find(var->name);
            if (fnd != global_vars.end()) {
                report_error(
                    token,
                    "A global variable with the name " + var->name + " already exists.");
                pass_error = true;
                return;
            }
        }

        auto global_param = std::dynamic_pointer_cast<GlobalParameterDescriptor>(var);
        if (global_param) {
            global_params[var->name] = global_param;
            return;
        }

        auto entry_pt = std::dynamic_pointer_cast<ShaderEntryPointDescriptor>(var);
        if (entry_pt) {
            entry_points[var->name] = entry_pt;
            return;
        }

        auto shader_fn = std::dynamic_pointer_cast<ShaderFunctionDescriptor>(var);
        if (shader_fn) {
            functions[var->name] = shader_fn;
            return;
        }

        // Otherwise it must be a plain global variable
        global_vars[var->name] = var;
    } else {
        auto &scope = scopes.back();
        auto fnd = scope.find(var->name);
        if (fnd != scope.end()) {
            report_error(token, "A variable with the name " + var->name + " already exists.");
            pass_error = true;
            return;
        }
        scope[var->name] = VariableStatus(var);
    }
}

void ResolverVisitor::define(const antlr4::Token *name)
{
    if (scopes.empty()) {
        return;
    }

    auto &scope = scopes.back();
    scope[name->getText()].defined = true;
}

std::shared_ptr<VariableDescriptor> ResolverVisitor::resolve_reference(
    antlr4::ParserRuleContext *node, const antlr4::Token *name)
{
    const std::string ident = name->getText();
    for (int i = scopes.size() - 1; i >= 0; --i) {
        auto &scope = scopes[i];
        auto fnd = scope.find(ident);
        if (fnd != scope.end()) {
            if (!fnd->second.defined) {
                pass_error = true;
                report_error(name, "Cannot read variable in its own initializer");
                return nullptr;
            } else {
                fnd->second.read = true;
                variable_access[node] = fnd->second.var_desc;
                return fnd->second.var_desc;
            }
        }
    }
    // Otherwise it's a global being accessed
    {
        auto fnd = global_params.find(ident);
        if (fnd != global_params.end()) {
            variable_access[node] = fnd->second;
            return fnd->second;
        }
    }
    {
        // TODO: Should it always be an error to call an entry point directly?
        // For now I think so, but just because of how they'll be rewritten.
        // They could be rewritten differently to split into an "entry point"
        // and "plain function", where the entry point just sets up the params
        // to call the plain function. Then direct calls to the entry point would
        // call the plain function.
        auto fnd = entry_points.find(ident);
        if (fnd != entry_points.end()) {
            report_error(name, "Shader entry points cannot be called directly as functions.");
            pass_error = true;
            return nullptr;
        }
    }
    {
        auto fnd = functions.find(ident);
        if (fnd != functions.end()) {
            variable_access[node] = fnd->second;
            return fnd->second;
        }
    }
    {
        auto fnd = global_vars.find(ident);
        if (fnd != global_vars.end()) {
            variable_access[node] = fnd->second;
            return fnd->second;
        }
    }
    pass_error = true;
    report_error(name, "Undefined variable: '" + ident + "'");
    return nullptr;
}

std::shared_ptr<StructDescriptor> ResolverVisitor::resolve_struct(const antlr4::Token *name)
{
    const std::string struct_name = name->getText();
    auto fnd = structs->find(struct_name);
    if (fnd == structs->end()) {
        pass_error = true;
        report_error(name, "No struct with the name '" + struct_name + "' was declared.");
        return nullptr;
    }
    return fnd->second;
}
}
