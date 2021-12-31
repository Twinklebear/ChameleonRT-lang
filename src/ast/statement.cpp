#include "statement.h"

namespace crtl {
namespace ast {
namespace stmt {

Statement::Statement(antlr4::Token *token, NodeType stmt_type) : Node(token, stmt_type) {}

Block::Block(antlr4::Token *token, const std::vector<std::shared_ptr<Statement>> &statements)
    : Statement(token, NodeType::STMT_BLOCK), statements(statements)
{
}

std::vector<Node *> Block::get_children()
{
    std::vector<Node *> children;
    for (auto &s : statements) {
        children.push_back(s.get());
    }
    return children;
}

IfElse::IfElse(antlr4::Token *token,
               const std::shared_ptr<expr::Expression> &condition,
               const std::shared_ptr<Statement> &if_branch,
               const std::shared_ptr<Statement> &else_branch)
    : Statement(token, NodeType::STMT_IF_ELSE),
      condition(condition),
      if_branch(if_branch),
      else_branch(else_branch)
{
}

std::vector<Node *> IfElse::get_children()
{
    std::vector<Node *> children;
    children.push_back(condition.get());
    children.push_back(if_branch.get());
    if (else_branch) {
        children.push_back(else_branch.get());
    }
    return children;
}

While::While(antlr4::Token *token,
             const std::shared_ptr<expr::Expression> &condition,
             const std::shared_ptr<Statement> &body)
    : Statement(token, NodeType::STMT_WHILE), condition(condition), body(body)
{
}

std::vector<Node *> While::get_children()
{
    std::vector<Node *> children;
    children.push_back(condition.get());
    if (body) {
        children.push_back(body.get());
    }
    return children;
}

For::For(antlr4::Token *token,
         const std::shared_ptr<Statement> &init,
         const std::shared_ptr<expr::Expression> &condition,
         const std::shared_ptr<expr::Expression> &advance,
         const std::shared_ptr<Statement> &body)
    : Statement(token, NodeType::STMT_FOR),
      init(init),
      condition(condition),
      advance(advance),
      body(body)
{
}

std::vector<Node *> For::get_children()
{
    std::vector<Node *> children;
    if (init) {
        children.push_back(init.get());
    }
    if (condition) {
        children.push_back(condition.get());
    }
    if (advance) {
        children.push_back(advance.get());
    }
    if (body) {
        children.push_back(body.get());
    }
    return children;
}

Return::Return(antlr4::Token *token, const std::shared_ptr<expr::Expression> &expr)
    : Statement(token, NodeType::STMT_RETURN), expression(expr)
{
}

std::vector<Node *> Return::get_children()
{
    std::vector<Node *> children;
    if (expression) {
        children.push_back(expression.get());
    }
    return children;
}

VariableDeclaration::VariableDeclaration(antlr4::Token *token,
                                         const std::shared_ptr<decl::Variable> &var_decl)
    : Statement(token, NodeType::STMT_VAR_DECL), var_decl(var_decl)
{
}

std::vector<Node *> VariableDeclaration::get_children()
{
    std::vector<Node *> children;
    children.push_back(var_decl.get());
    return children;
}

Expression::Expression(antlr4::Token *token, const std::shared_ptr<expr::Expression> &expr)
    : Statement(token, NodeType::STMT_EXPR), expr(expr)
{
}

std::vector<Node *> Expression::get_children()
{
    std::vector<Node *> children;
    children.push_back(expr.get());
    return children;
}
}
}
}
