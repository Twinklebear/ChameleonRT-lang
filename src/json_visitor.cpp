#include "json_visitor.h"

namespace crtl {

using namespace ast;

std::any JSONVisitor::visit_ast(const std::shared_ptr<AST> &ast)
{
    std::vector<nlohmann::json> results;
    for (auto &n : ast->top_level_decls) {
        results.push_back(std::any_cast<nlohmann::json>(visit(n)));
    }
    ast_json = results;
    return ast_json;
}

std::any JSONVisitor::visit_decl_function(const std::shared_ptr<decl::Function> &d)
{
    nlohmann::json d_json;
    const auto sym = d->get_symbol();
    d_json["ast_node"] = "ast::decl::Function";
    d_json["name"] = sym->name;
    d_json["generated"] = sym->is_generated();
    if (!sym->is_generated()) {
        d_json["line"] = sym->token->getLine();
    }
    d_json["type"] = d->get_type()->to_string();

    auto children = d->get_children();
    // Get a list of just the parameters to visit
    std::vector<std::shared_ptr<Node>> parameters;
    std::shared_ptr<Node> block = nullptr;
    for (auto &c : children) {
        if (std::dynamic_pointer_cast<decl::Variable>(c)) {
            parameters.push_back(c);
        } else if (std::dynamic_pointer_cast<stmt::Block>(c)) {
            block = c;
        }
    }
    d_json["parameters"] = visit_all(parameters);
    d_json["block"] = std::any_cast<nlohmann::json>(visit(block));
    return d_json;
}

std::any JSONVisitor::visit_decl_entry_point(const std::shared_ptr<decl::EntryPoint> &d)
{
    const auto sym = d->get_symbol();

    nlohmann::json d_json;
    d_json["ast_node"] = "ast::decl::EntryPoint";
    d_json["type"] = d->get_type()->to_string();
    d_json["name"] = sym->name;
    d_json["generated"] = sym->is_generated();
    if (!sym->is_generated()) {
        d_json["line"] = sym->token->getLine();
    }

    auto children = d->get_children();
    // Get a list of just the parameters to visit
    std::vector<std::shared_ptr<Node>> parameters;
    std::shared_ptr<Node> block = nullptr;
    for (auto &c : children) {
        if (std::dynamic_pointer_cast<decl::Variable>(c)) {
            parameters.push_back(c);
        } else if (std::dynamic_pointer_cast<stmt::Block>(c)) {
            block = c;
        }
    }
    d_json["parameters"] = visit_all(parameters);
    d_json["block"] = std::any_cast<nlohmann::json>(visit(block));
    return d_json;
}

std::any JSONVisitor::visit_decl_global_param(const std::shared_ptr<decl::GlobalParam> &d)
{
    const auto sym = d->get_symbol();

    nlohmann::json d_json;
    d_json["ast_node"] = "ast::decl::GlobalParam";
    d_json["type"] = d->get_type()->to_string();
    d_json["name"] = sym->name;
    d_json["generated"] = sym->is_generated();
    if (!sym->is_generated()) {
        d_json["line"] = sym->token->getLine();
    }

    return d_json;
}

std::any JSONVisitor::visit_decl_struct(const std::shared_ptr<decl::Struct> &d)
{
    const auto sym = d->get_symbol();

    nlohmann::json d_json;
    d_json["ast_node"] = "ast::decl::Struct";
    d_json["name"] = sym->name;
    d_json["generated"] = sym->is_generated();
    if (!sym->is_generated()) {
        d_json["line"] = sym->token->getLine();
    }
    d_json["members"] = visit_all(d->get_children());

    return d_json;
}

std::any JSONVisitor::visit_decl_struct_member(const std::shared_ptr<decl::StructMember> &d)
{
    const auto sym = d->get_symbol();

    nlohmann::json d_json;
    d_json["ast_node"] = "ast::decl::StructMember";
    d_json["type"] = d->get_type()->to_string();
    d_json["name"] = sym->name;
    d_json["generated"] = sym->is_generated();
    if (!sym->is_generated()) {
        d_json["line"] = sym->token->getLine();
    }

    return d_json;
}

std::any JSONVisitor::visit_decl_variable(const std::shared_ptr<decl::Variable> &d)
{
    const auto sym = d->get_symbol();

    nlohmann::json d_json;
    d_json["ast_node"] = "ast::decl::Variable";
    d_json["type"] = d->get_type()->to_string();
    d_json["name"] = sym->name;
    d_json["generated"] = sym->is_generated();
    if (!sym->is_generated()) {
        d_json["line"] = sym->token->getLine();
    }
    if (d->expression) {
        d_json["initializer"] = std::any_cast<nlohmann::json>(visit(d->expression));
    }

    return d_json;
}

std::any JSONVisitor::visit_stmt_block(const std::shared_ptr<stmt::Block> &s)
{
    nlohmann::json s_json;

    s_json["ast_node"] = "ast::stmt::Block";
    s_json["generated"] = s->is_generated();
    if (!s->is_generated()) {
        s_json["line"] = s->get_token()->getLine();
    }
    s_json["statements"] = visit_all(s->get_children());

    return s_json;
}

std::any JSONVisitor::visit_stmt_if_else(const std::shared_ptr<stmt::IfElse> &s)
{
    nlohmann::json s_json;

    s_json["ast_node"] = "ast::stmt::IfElse";
    s_json["generated"] = s->is_generated();
    if (!s->is_generated()) {
        s_json["line"] = s->get_token()->getLine();
    }
    s_json["condition"] = std::any_cast<nlohmann::json>(visit(s->condition));
    s_json["if_branch"] = std::any_cast<nlohmann::json>(visit(s->if_branch));
    if (s->else_branch) {
        s_json["else_branch"] = std::any_cast<nlohmann::json>(visit(s->if_branch));
    }

    return s_json;
}

std::any JSONVisitor::visit_stmt_while(const std::shared_ptr<stmt::While> &s)
{
    nlohmann::json s_json;

    s_json["ast_node"] = "ast::stmt::While";
    s_json["generated"] = s->is_generated();
    if (!s->is_generated()) {
        s_json["line"] = s->get_token()->getLine();
    }
    s_json["condition"] = std::any_cast<nlohmann::json>(visit(s->condition));
    if (s->body) {
        s_json["body"] = std::any_cast<nlohmann::json>(visit(s->body));
    }

    return s_json;
}

std::any JSONVisitor::visit_stmt_for(const std::shared_ptr<stmt::For> &s)
{
    nlohmann::json s_json;

    s_json["ast_node"] = "ast::stmt::For";
    s_json["generated"] = s->is_generated();
    if (!s->is_generated()) {
        s_json["line"] = s->get_token()->getLine();
    }

    if (s->init) {
        s_json["init"] = std::any_cast<nlohmann::json>(visit(s->init));
    }
    if (s->condition) {
        s_json["condition"] = std::any_cast<nlohmann::json>(visit(s->condition));
    }
    if (s->advance) {
        s_json["advance"] = std::any_cast<nlohmann::json>(visit(s->advance));
    }

    return s_json;
}

std::any JSONVisitor::visit_stmt_return(const std::shared_ptr<stmt::Return> &s)
{
    nlohmann::json s_json;

    s_json["ast_node"] = "ast::stmt::Return";
    s_json["generated"] = s->is_generated();
    if (!s->is_generated()) {
        s_json["line"] = s->get_token()->getLine();
    }
    if (s->expression) {
        s_json["expression"] = std::any_cast<nlohmann::json>(visit(s->expression));
    }

    return s_json;
}

std::any JSONVisitor::visit_stmt_variable_declaration(
    const std::shared_ptr<stmt::VariableDeclaration> &s)
{
    nlohmann::json s_json;

    s_json["ast_node"] = "ast::stmt::VariableDeclaration";
    s_json["generated"] = s->is_generated();
    if (!s->is_generated()) {
        s_json["line"] = s->get_token()->getLine();
    }
    s_json["var"] = std::any_cast<nlohmann::json>(visit(s->var_decl));

    return s_json;
}

std::any JSONVisitor::visit_stmt_expression(const std::shared_ptr<stmt::Expression> &s)
{
    nlohmann::json s_json;

    s_json["ast_node"] = "ast::stmt::Expression";
    s_json["generated"] = s->is_generated();
    if (!s->is_generated()) {
        s_json["line"] = s->get_token()->getLine();
    }
    s_json["expr"] = std::any_cast<nlohmann::json>(visit(s->expr));

    return s_json;
}

std::any JSONVisitor::visit_expr_unary(const std::shared_ptr<expr::Unary> &e)
{
    nlohmann::json e_json;

    e_json["ast_node"] = "ast::expr::Unary";
    e_json["generated"] = e->is_generated();
    if (!e->is_generated()) {
        e_json["line"] = e->get_token()->getLine();
    }
    e_json["op"] = ast::to_string(e->get_node_type());
    e_json["expr"] = std::any_cast<nlohmann::json>(visit(e->expr));

    return e_json;
}

std::any JSONVisitor::visit_expr_binary(const std::shared_ptr<expr::Binary> &e)
{
    nlohmann::json e_json;

    e_json["ast_node"] = "ast::expr::Binary";
    e_json["generated"] = e->is_generated();
    if (!e->is_generated()) {
        e_json["line"] = e->get_token()->getLine();
    }
    e_json["op"] = ast::to_string(e->get_node_type());
    e_json["left"] = std::any_cast<nlohmann::json>(visit(e->left));
    e_json["right"] = std::any_cast<nlohmann::json>(visit(e->right));

    return e_json;
}

std::any JSONVisitor::visit_expr_variable(const std::shared_ptr<expr::Variable> &e)
{
    nlohmann::json e_json;

    e_json["ast_node"] = "ast::expr::Variable";
    e_json["generated"] = e->is_generated();
    if (!e->is_generated()) {
        e_json["line"] = e->get_token()->getLine();
    }
    e_json["name"] = e->name();

    return e_json;
}

std::any JSONVisitor::visit_expr_constant(const std::shared_ptr<expr::Constant> &e)
{
    nlohmann::json e_json;

    e_json["ast_node"] = "ast::expr::Constant";
    e_json["generated"] = e->is_generated();
    if (!e->is_generated()) {
        e_json["line"] = e->get_token()->getLine();
    }
    e_json["type"] = ty::to_string(e->constant_type);
    switch (e->constant_type) {
    case ty::PrimitiveType::BOOL:
        e_json["value"] = std::any_cast<bool>(e->value);
        break;
    case ty::PrimitiveType::INT:
        e_json["value"] = std::any_cast<int>(e->value);
        break;
    case ty::PrimitiveType::UINT:
        e_json["value"] = std::any_cast<uint32_t>(e->value);
        break;
    case ty::PrimitiveType::FLOAT:
        e_json["value"] = std::any_cast<float>(e->value);
        break;
    case ty::PrimitiveType::DOUBLE:
        e_json["value"] = std::any_cast<double>(e->value);
        break;
    default:
        e_json["value"] = "INVALID/VOID!?";
        break;
    }
    return e_json;
}

std::any JSONVisitor::visit_expr_function_call(const std::shared_ptr<expr::FunctionCall> &e)
{
    nlohmann::json e_json;

    e_json["ast_node"] = "ast::expr::FunctionCall";
    e_json["generated"] = e->is_generated();
    if (!e->is_generated()) {
        e_json["line"] = e->get_token()->getLine();
    }
    e_json["op"] = ast::to_string(e->get_node_type());
    e_json["callee"] = e->get_text();

    std::vector<std::shared_ptr<Node>> args;
    std::transform(e->args.begin(),
                   e->args.end(),
                   std::back_inserter(args),
                   [](std::shared_ptr<expr::Expression> &a) {
                       return std::dynamic_pointer_cast<Node>(a);
                   });
    e_json["args"] = visit_all(args);

    return e_json;
}

std::any JSONVisitor::visit_struct_array_access(
    const std::shared_ptr<expr::StructArrayAccess> &e)
{
    nlohmann::json e_json;

    e_json["ast_node"] = "ast::expr::StructArrayAccess";
    e_json["generated"] = e->is_generated();
    if (!e->is_generated()) {
        e_json["line"] = e->get_token()->getLine();
    }
    e_json["op"] = ast::to_string(e->get_node_type());
    e_json["variable"] = std::any_cast<nlohmann::json>(visit(e->variable));
    e_json["struct_array_access_chain"] = visit_struct_array_fragments(e->struct_array_access);

    return e_json;
}

std::any JSONVisitor::visit_expr_assignment(const std::shared_ptr<expr::Assignment> &e)
{
    nlohmann::json e_json;

    e_json["ast_node"] = "ast::expr::Assignment";
    e_json["generated"] = e->is_generated();
    if (!e->is_generated()) {
        e_json["line"] = e->get_token()->getLine();
    }
    e_json["op"] = ast::to_string(e->get_node_type());
    e_json["value"] = std::any_cast<nlohmann::json>(visit(e->value));
    e_json["lhs"] = std::any_cast<nlohmann::json>(visit(e->lhs));

    return e_json;
}

std::vector<nlohmann::json> JSONVisitor::visit_all(
    const std::vector<std::shared_ptr<Node>> &nodes)
{
    std::vector<nlohmann::json> results;
    for (auto &n : nodes) {
        results.push_back(std::any_cast<nlohmann::json>(visit(n)));
    }
    return results;
}

std::vector<nlohmann::json> JSONVisitor::visit_struct_array_fragments(
    const std::vector<std::shared_ptr<expr::StructArrayAccessFragment>> &fragments)
{
    std::vector<nlohmann::json> results;
    for (auto &f : fragments) {
        auto struct_fragment = std::dynamic_pointer_cast<expr::StructMemberAccessFragment>(f);
        if (struct_fragment) {
            nlohmann::json j;
            j["ast_node"] = "ast::expr::StructMemberAccessFragment";
            j["member_name"] = struct_fragment->member->getText();
            results.push_back(j);
        } else {
            auto array_fragment = std::dynamic_pointer_cast<expr::ArrayAccessFragment>(f);
            nlohmann::json j;
            j["ast_node"] = "ast::expr::ArrayAccessFragment";
            j["expr"] = std::any_cast<nlohmann::json>(visit(array_fragment->index));
            results.push_back(j);
        }
    }
    return results;
}
}
