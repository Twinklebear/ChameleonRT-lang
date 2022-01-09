#pragma once

#include <any>
#include "node.h"
#include "type.h"

namespace crtl {
namespace ast {
namespace expr {

class Expression : public Node {
public:
    Expression(antlr4::Token *token, NodeType expr_type);
};

class Unary : public Expression {
public:
    std::shared_ptr<Expression> expr;

    Unary(antlr4::Token *op, NodeType expr_type, const std::shared_ptr<Expression> &expr);

    static std::shared_ptr<Unary> negate(antlr4::Token *op,
                                         const std::shared_ptr<Expression> &expr);

    static std::shared_ptr<Unary> logic_not(antlr4::Token *op,
                                            const std::shared_ptr<Expression> &expr);

    std::vector<Node *> get_children() override;
};

class Binary : public Expression {
public:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;

    Binary(antlr4::Token *op,
           NodeType expr_type,
           const std::shared_ptr<Expression> &left,
           const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> multiply(antlr4::Token *op,
                                            const std::shared_ptr<Expression> &left,
                                            const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> divide(antlr4::Token *op,
                                          const std::shared_ptr<Expression> &left,
                                          const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> add(antlr4::Token *op,
                                       const std::shared_ptr<Expression> &left,
                                       const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> subtract(antlr4::Token *op,
                                            const std::shared_ptr<Expression> &left,
                                            const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> cmp_less(antlr4::Token *op,
                                            const std::shared_ptr<Expression> &left,
                                            const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> cmp_less_equal(antlr4::Token *op,
                                                  const std::shared_ptr<Expression> &left,
                                                  const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> cmp_greater(antlr4::Token *op,
                                               const std::shared_ptr<Expression> &left,
                                               const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> cmp_greater_equal(antlr4::Token *op,
                                                     const std::shared_ptr<Expression> &left,
                                                     const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> cmp_not_equal(antlr4::Token *op,
                                                 const std::shared_ptr<Expression> &left,
                                                 const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> cmp_equal(antlr4::Token *op,
                                             const std::shared_ptr<Expression> &left,
                                             const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> logic_and(antlr4::Token *op,
                                             const std::shared_ptr<Expression> &left,
                                             const std::shared_ptr<Expression> &right);

    static std::shared_ptr<Binary> logic_or(antlr4::Token *op,
                                            const std::shared_ptr<Expression> &left,
                                            const std::shared_ptr<Expression> &right);

    std::vector<Node *> get_children() override;
};

class Variable : public Expression {
public:
    Variable(antlr4::Token *var);

    std::string name() const;

    std::vector<Node *> get_children() override;
};

/* A compile time constant value
 */
class Constant : public Expression {
public:
    std::any value;
    ty::PrimitiveType constant_type;

    Constant(antlr4::Token *constant, bool value);
    Constant(antlr4::Token *constant, int value);
    // TODO: unsigned int constants?
    Constant(antlr4::Token *constant, float value);
    Constant(antlr4::Token *constant, double value);

    std::vector<Node *> get_children() override;
};

class StructArrayAccessFragment {
public:
    virtual ~StructArrayAccessFragment() = default;
};

class StructMemberAccessFragment : public StructArrayAccessFragment {
public:
    antlr4::Token *member;

    StructMemberAccessFragment(antlr4::Token *member);
};

class ArrayAccessFragment : public StructArrayAccessFragment {
public:
    std::shared_ptr<Expression> index;

    ArrayAccessFragment(const std::shared_ptr<Expression> &index);
};

class FunctionCall : public Expression {
public:
    std::vector<std::shared_ptr<Expression>> args;
    // Any struct member or array accesses performed on the return value of the call
    std::vector<std::shared_ptr<StructArrayAccessFragment>> struct_array_access;

    FunctionCall(antlr4::Token *callee, const std::vector<std::shared_ptr<Expression>> &args);

    std::vector<Node *> get_children() override;
};

class StructArrayAccess : public Expression {
public:
    std::shared_ptr<Variable> variable;
    std::vector<std::shared_ptr<StructArrayAccessFragment>> struct_array_access;

    StructArrayAccess(
        const std::shared_ptr<Variable> &variable,
        const std::vector<std::shared_ptr<StructArrayAccessFragment>> &struct_array_access);

    std::vector<Node *> get_children() override;
};

class Assignment : public Expression {
public:
    // The left hand side will either be a plain expr::Variable or a expr::StructArrayAccess
    std::shared_ptr<Expression> lhs;
    std::shared_ptr<Expression> value;

    Assignment(const std::shared_ptr<Expression> &lhs,
               const std::shared_ptr<Expression> &value);

    std::vector<Node *> get_children() override;
};

}
}
}
