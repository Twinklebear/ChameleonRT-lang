#include "declaration.h"
#include "statement.h"

namespace crtl {
namespace ast {
namespace decl {

Declaration::Declaration(const std::string &name,
                         antlr4::Token *token,
                         const std::shared_ptr<ty::Type> &type,
                         NodeType decl_type)
    : Node(token, decl_type), symbol(std::make_shared<Symbol>(name, token))
{
    symbol->type = type;
}

const std::shared_ptr<ty::Type> Declaration::get_type() const
{
    return symbol->type;
}

std::shared_ptr<ty::Type> Declaration::get_type()
{
    return symbol->type;
}

const std::shared_ptr<Symbol> Declaration::get_symbol() const
{
    return symbol;
}

std::shared_ptr<Symbol> Declaration::get_symbol()
{
    return symbol;
}

std::shared_ptr<ty::Type> make_function_type(
    const std::vector<std::shared_ptr<Variable>> &parameters,
    const std::shared_ptr<ty::Type> &return_type)
{
    // Pull the types of each parameter into its own vector
    std::vector<std::shared_ptr<ty::Type>> param_types;
    for (const auto &p : parameters) {
        param_types.push_back(p->get_type());
    }
    return std::make_shared<ty::Function>(param_types, return_type);
}

Function::Function(const std::string &name,
                   antlr4::Token *token,
                   const std::vector<std::shared_ptr<Variable>> &parameters,
                   const std::shared_ptr<stmt::Block> &block,
                   const std::shared_ptr<ty::Type> &return_type)
    : Declaration(
          name, token, make_function_type(parameters, return_type), NodeType::DECL_FCN),
      parameters(parameters),
      block(block)
{
}

std::vector<Node *> Function::get_children()
{
    std::vector<Node *> children;
    for (auto &p : parameters) {
        children.push_back(p.get());
    }
    children.push_back(block.get());
    return children;
}

std::shared_ptr<ty::Type> make_entry_point_type(
    const std::vector<std::shared_ptr<Variable>> &parameters,
    const ty::EntryPointType entry_pt_type)
{
    // Pull the types of each parameter into its own vector
    std::vector<std::shared_ptr<ty::Type>> param_types;
    for (const auto &p : parameters) {
        param_types.push_back(p->get_type());
    }
    return std::make_shared<ty::EntryPoint>(param_types, entry_pt_type);
}

EntryPoint::EntryPoint(const std::string &name,
                       antlr4::Token *token,
                       const std::vector<std::shared_ptr<Variable>> &parameters,
                       const ty::EntryPointType entry_pt_type,
                       const std::shared_ptr<stmt::Block> &block)
    : Declaration(name,
                  token,
                  make_entry_point_type(parameters, entry_pt_type),
                  NodeType::DECL_ENTRY_POINT),
      parameters(parameters),
      block(block)
{
}

std::vector<Node *> EntryPoint::get_children()
{
    std::vector<Node *> children;
    for (auto &p : parameters) {
        children.push_back(p.get());
    }
    children.push_back(block.get());
    return children;
}

GlobalParam::GlobalParam(const std::string &name,
                         antlr4::Token *token,
                         const std::shared_ptr<ty::Type> &type)
    : Variable(name, token, type)
{
    // Set the right node type for the child class
    node_type = NodeType::DECL_GLOBAL_PARAM;
}

std::vector<Node *> GlobalParam::get_children()
{
    return std::vector<Node *>();
}

StructMember::StructMember(const std::string &name,
                           antlr4::Token *token,
                           const std::shared_ptr<ty::Type> &type)
    : Declaration(name, token, type, NodeType::DECL_STRUCT_MEMBER)
{
}

std::vector<Node *> StructMember::get_children()
{
    return std::vector<Node *>();
}

Struct::Struct(const std::string &name,
               antlr4::Token *token,
               const std::vector<std::shared_ptr<StructMember>> &members)
    : Declaration(name, token, std::make_shared<ty::Struct>(name), NodeType::DECL_STRUCT),
      members(members)
{
}

std::vector<Node *> Struct::get_children()
{
    std::vector<Node *> children;
    for (auto &m : members) {
        children.push_back(m.get());
    }
    return children;
}

Variable::Variable(const std::string &name,
                   antlr4::Token *token,
                   const std::shared_ptr<ty::Type> &type,
                   const std::shared_ptr<expr::Expression> &expression)
    : Declaration(name, token, type, NodeType::DECL_VAR), expression(expression)
{
}

std::vector<Node *> Variable::get_children()
{
    std::vector<Node *> children;
    if (expression) {
        children.push_back(expression.get());
    }
    return children;
}
}
}
}
