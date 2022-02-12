#include "expression.h"
#include <stdexcept>

namespace crtl {
namespace ast {
namespace expr {

std::string operator_to_string(const NodeType nt)
{
    switch (nt) {
    case NodeType::EXPR_NEGATE:
        return "-";
    case NodeType::EXPR_LOGIC_NOT:
        return "!";
    case NodeType::EXPR_MULT:
        return "*";
    case NodeType::EXPR_DIV:
        return "/";
    case NodeType::EXPR_ADD:
        return "+";
    case NodeType::EXPR_SUB:
        return "-";
    case NodeType::EXPR_CMP_LESS:
        return "<";
    case NodeType::EXPR_CMP_LESS_EQUAL:
        return "<=";
    case NodeType::EXPR_CMP_GREATER:
        return ">";
    case NodeType::EXPR_CMP_GREATER_EQUAL:
        return ">=";
    case NodeType::EXPR_CMP_NOT_EQUAL:
        return "!=";
    case NodeType::EXPR_CMP_EQUAL:
        return "==";
    case NodeType::EXPR_LOGIC_AND:
        return "&&";
    case NodeType::EXPR_LOGIC_OR:
        return "||";
    default:
        break;
    }

    throw std::runtime_error(
        "Invalid NodeType passed to operator_to_string: not an expression!");
    return "INVALID";
}

Expression::Expression(antlr4::Token *token, NodeType expr_type) : Node(token, expr_type) {}

Unary::Unary(antlr4::Token *op, NodeType expr_type, const std::shared_ptr<Expression> &expr)
    : Expression(op, expr_type), expr(expr)
{
}

std::shared_ptr<Unary> Unary::negate(antlr4::Token *op,
                                     const std::shared_ptr<Expression> &expr)
{
    return std::make_shared<Unary>(op, NodeType::EXPR_NEGATE, expr);
}

std::shared_ptr<Unary> Unary::logic_not(antlr4::Token *op,
                                        const std::shared_ptr<Expression> &expr)
{
    return std::make_shared<Unary>(op, NodeType::EXPR_LOGIC_NOT, expr);
}

std::vector<std::shared_ptr<Node>> Unary::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    children.push_back(expr);
    return children;
}

std::string Unary::operator_string() const
{
    return operator_to_string(node_type);
}

Binary::Binary(antlr4::Token *op,
               NodeType expr_type,
               const std::shared_ptr<Expression> &left,
               const std::shared_ptr<Expression> &right)
    : Expression(op, expr_type), left(left), right(right)
{
}

std::shared_ptr<Binary> Binary::multiply(antlr4::Token *op,
                                         const std::shared_ptr<Expression> &left,
                                         const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_MULT, left, right);
}

std::shared_ptr<Binary> Binary::divide(antlr4::Token *op,
                                       const std::shared_ptr<Expression> &left,
                                       const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_DIV, left, right);
}

std::shared_ptr<Binary> Binary::add(antlr4::Token *op,
                                    const std::shared_ptr<Expression> &left,
                                    const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_ADD, left, right);
}

std::shared_ptr<Binary> Binary::subtract(antlr4::Token *op,
                                         const std::shared_ptr<Expression> &left,
                                         const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_SUB, left, right);
}

std::shared_ptr<Binary> Binary::cmp_less(antlr4::Token *op,
                                         const std::shared_ptr<Expression> &left,
                                         const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_CMP_LESS, left, right);
}

std::shared_ptr<Binary> Binary::cmp_less_equal(antlr4::Token *op,
                                               const std::shared_ptr<Expression> &left,
                                               const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_CMP_LESS_EQUAL, left, right);
}

std::shared_ptr<Binary> Binary::cmp_greater(antlr4::Token *op,
                                            const std::shared_ptr<Expression> &left,
                                            const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_CMP_GREATER, left, right);
}

std::shared_ptr<Binary> Binary::cmp_greater_equal(antlr4::Token *op,
                                                  const std::shared_ptr<Expression> &left,
                                                  const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_CMP_GREATER_EQUAL, left, right);
}

std::shared_ptr<Binary> Binary::cmp_not_equal(antlr4::Token *op,
                                              const std::shared_ptr<Expression> &left,
                                              const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_CMP_NOT_EQUAL, left, right);
}

std::shared_ptr<Binary> Binary::cmp_equal(antlr4::Token *op,
                                          const std::shared_ptr<Expression> &left,
                                          const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_CMP_EQUAL, left, right);
}

std::shared_ptr<Binary> Binary::logic_and(antlr4::Token *op,
                                          const std::shared_ptr<Expression> &left,
                                          const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_LOGIC_AND, left, right);
}

std::shared_ptr<Binary> Binary::logic_or(antlr4::Token *op,
                                         const std::shared_ptr<Expression> &left,
                                         const std::shared_ptr<Expression> &right)
{
    return std::make_shared<Binary>(op, NodeType::EXPR_LOGIC_OR, left, right);
}

std::vector<std::shared_ptr<Node>> Binary::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    children.push_back(left);
    children.push_back(right);
    return children;
}

std::string Binary::operator_string() const
{
    return operator_to_string(node_type);
}

Variable::Variable(antlr4::Token *var)
    : Expression(var, NodeType::EXPR_LITERAL_VAR), var_name(var->getText())
{
}

Variable::Variable(const std::string &name)
    : Expression(nullptr, NodeType::EXPR_LITERAL_VAR), var_name(name)
{
}

std::string Variable::name() const
{
    return var_name;
}

std::vector<std::shared_ptr<Node>> Variable::get_children()
{
    return std::vector<std::shared_ptr<Node>>();
}

Constant::Constant(antlr4::Token *constant, bool value)
    : Expression(constant, NodeType::EXPR_LITERAL_CONSTANT),
      value(value),
      constant_type(ty::PrimitiveType::BOOL)
{
}

Constant::Constant(antlr4::Token *constant, int value)
    : Expression(constant, NodeType::EXPR_LITERAL_CONSTANT),
      value(value),
      constant_type(ty::PrimitiveType::INT)
{
}

Constant::Constant(antlr4::Token *constant, float value)
    : Expression(constant, NodeType::EXPR_LITERAL_CONSTANT),
      value(value),
      constant_type(ty::PrimitiveType::FLOAT)
{
}

Constant::Constant(antlr4::Token *constant, double value)
    : Expression(constant, NodeType::EXPR_LITERAL_CONSTANT),
      value(value),
      constant_type(ty::PrimitiveType::DOUBLE)
{
}

std::vector<std::shared_ptr<Node>> Constant::get_children()
{
    return std::vector<std::shared_ptr<Node>>();
}

StructMemberAccessFragment::StructMemberAccessFragment(antlr4::Token *member) : member(member)
{
}

std::string StructMemberAccessFragment::name() const
{
    return member->getText();
}

ArrayAccessFragment::ArrayAccessFragment(const std::shared_ptr<Expression> &index)
    : index(index)
{
}

FunctionCall::FunctionCall(antlr4::Token *callee,
                           const std::vector<std::shared_ptr<Expression>> &args)
    : Expression(callee, NodeType::EXPR_FCN_CALL), args(args)
{
}

StructArrayAccess::StructArrayAccess(
    const std::shared_ptr<Variable> &var,
    const std::vector<std::shared_ptr<StructArrayAccessFragment>> &struct_array_access)
    : Expression(var->get_token(), NodeType::EXPR_STRUCT_ARRAY_ACCESS),
      variable(var),
      struct_array_access(struct_array_access)
{
}

std::vector<std::shared_ptr<Node>> StructArrayAccess::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    children.push_back(variable);
    return children;
}

std::vector<std::shared_ptr<Node>> FunctionCall::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    for (auto &a : args) {
        children.push_back(a);
    }
    return children;
}

Assignment::Assignment(const std::shared_ptr<Expression> &leftside,
                       const std::shared_ptr<Expression> &value)
    : Expression(leftside->get_token(), NodeType::EXPR_ASSIGN), lhs(leftside), value(value)
{
}

std::vector<std::shared_ptr<Node>> Assignment::get_children()
{
    std::vector<std::shared_ptr<Node>> children;
    children.push_back(lhs);
    children.push_back(value);
    return children;
}

}
}
}
