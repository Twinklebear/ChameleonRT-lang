#pragma once

#include "expression.h"
#include "node.h"
#include "symbol.h"
#include "type.h"

namespace crtl {
namespace ast {

namespace stmt {
class Block;
}

namespace decl {

// A declaration introduces a symbol
class Declaration : public Node {
protected:
    std::shared_ptr<Symbol> symbol;

public:
    Declaration(const std::string &name,
                antlr4::Token *token,
                const std::shared_ptr<ty::Type> &type,
                NodeType decl_type);

    const std::shared_ptr<ty::Type> &get_type() const;

    std::shared_ptr<ty::Type> &get_type();

    const std::shared_ptr<Symbol> &get_symbol() const;

    std::shared_ptr<Symbol> &get_symbol();

    std::string get_text() const override;
};

class Variable : public Declaration {
public:
    std::shared_ptr<expr::Expression> expression;

    Variable(const std::string &name,
             antlr4::Token *token,
             const std::shared_ptr<ty::Type> &type,
             const std::shared_ptr<expr::Expression> &expression = nullptr);

    std::vector<std::shared_ptr<Node>> get_children() override;
};

class Function : public Declaration {
public:
    std::vector<std::shared_ptr<Variable>> parameters;
    std::shared_ptr<stmt::Block> block;

    // Create a declaration for a user/source code declared function
    Function(const std::string &name,
             antlr4::Token *token,
             const std::vector<std::shared_ptr<Variable>> &parameters,
             const std::shared_ptr<stmt::Block> &block,
             const std::shared_ptr<ty::Type> &return_type);

    // Create a declaration for a built in function
    Function(const std::string &name,
             const std::vector<std::shared_ptr<Variable>> &parameters,
             const std::shared_ptr<ty::Type> &return_type);

    std::vector<std::shared_ptr<Node>> get_children() override;

    // If the function declaration is for a built-in "intrinsic" function,
    // or a user-declared function
    bool is_builtin() const;
};

class EntryPoint : public Declaration {
public:
    std::vector<std::shared_ptr<Variable>> parameters;
    std::shared_ptr<stmt::Block> block;

    EntryPoint(const std::string &name,
               antlr4::Token *token,
               const std::vector<std::shared_ptr<Variable>> &parameters,
               const ty::EntryPointType type,
               const std::shared_ptr<stmt::Block> &block);

    std::vector<std::shared_ptr<Node>> get_children() override;
};

class GlobalParam : public Variable {
public:
    GlobalParam(const std::string &name,
                antlr4::Token *token,
                const std::shared_ptr<ty::Type> &type);

    std::vector<std::shared_ptr<Node>> get_children() override;
};

class StructMember : public Declaration {
public:
    StructMember(const std::string &name,
                 antlr4::Token *token,
                 const std::shared_ptr<ty::Type> &type);

    std::vector<std::shared_ptr<Node>> get_children() override;
};

class Struct : public Declaration {
public:
    std::vector<std::shared_ptr<StructMember>> members;

    Struct(const std::string &name,
           antlr4::Token *token,
           const std::vector<std::shared_ptr<StructMember>> &members);

    std::shared_ptr<StructMember> get_member(const std::string &name);

    std::vector<std::shared_ptr<Node>> get_children() override;
};

}
}
}
