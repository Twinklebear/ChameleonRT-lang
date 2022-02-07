#include "statement.h"

namespace crtl {
namespace ast {
namespace stmt {

Statement::Statement(antlr4::Token *token, NodeType stmt_type) : Node(token, stmt_type) {}

Block::Block(antlr4::Token *token, const std::vector<std::shared_ptr<Statement>> &statements)
    : Statement(token, NodeType::STMT_BLOCK), statements(statements)
{
}

std::vector<std::shared_ptr<Node>> Block::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    for (auto &s : statements) {
        children.push_back(s);
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

std::vector<std::shared_ptr<Node>> IfElse::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    children.push_back(condition);
    children.push_back(if_branch);
    if (else_branch) {
        children.push_back(else_branch);
    }
    return children;
}

While::While(antlr4::Token *token,
             const std::shared_ptr<expr::Expression> &condition,
             const std::shared_ptr<Statement> &body)
    : Statement(token, NodeType::STMT_WHILE), condition(condition), body(body)
{
}

std::vector<std::shared_ptr<Node>> While::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    children.push_back(condition);
    if (body) {
        children.push_back(body);
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

std::vector<std::shared_ptr<Node>> For::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    if (init) {
        children.push_back(init);
    }
    if (condition) {
        children.push_back(condition);
    }
    if (advance) {
        children.push_back(advance);
    }
    if (body) {
        children.push_back(body);
    }
    return children;
}

Return::Return(antlr4::Token *token, const std::shared_ptr<expr::Expression> &expr)
    : Statement(token, NodeType::STMT_RETURN), expression(expr)
{
}

std::vector<std::shared_ptr<Node>> Return::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    if (expression) {
        children.push_back(expression);
    }
    return children;
}

VariableDeclaration::VariableDeclaration(antlr4::Token *token,
                                         const std::shared_ptr<decl::Variable> &var_decl)
    : Statement(token, NodeType::STMT_VAR_DECL), var_decl(var_decl)
{
}

std::vector<std::shared_ptr<Node>> VariableDeclaration::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    children.push_back(var_decl);
    return children;
}

Expression::Expression(antlr4::Token *token, const std::shared_ptr<expr::Expression> &expr)
    : Statement(token, NodeType::STMT_EXPR), expr(expr)
{
}

std::vector<std::shared_ptr<Node>> Expression::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    children.push_back(expr);
    return children;
}
}
}
}
