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

    const std::shared_ptr<ty::Type> get_type() const;

    std::shared_ptr<ty::Type> get_type();

    const std::shared_ptr<Symbol> get_symbol() const;

    std::shared_ptr<Symbol> get_symbol();
};

class Variable : public Declaration {
public:
    std::shared_ptr<expr::Expression> expression;

    Variable(const std::string &name,
             antlr4::Token *token,
             const std::shared_ptr<ty::Type> &type,
             const std::shared_ptr<expr::Expression> &expression = nullptr);

    std::vector<Node *> get_children() override;
};

class Function : public Declaration {
public:
    std::vector<std::shared_ptr<Variable>> parameters;
    std::shared_ptr<stmt::Block> block;

    Function(const std::string &name,
             antlr4::Token *token,
             const std::vector<std::shared_ptr<Variable>> &parameters,
             const std::shared_ptr<stmt::Block> &block,
             const std::shared_ptr<ty::Type> &return_type);

    std::vector<Node *> get_children() override;
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

    std::vector<Node *> get_children() override;
};

class GlobalParam : public Variable {
public:
    GlobalParam(const std::string &name,
                antlr4::Token *token,
                const std::shared_ptr<ty::Type> &type);

    std::vector<Node *> get_children() override;
};

class StructMember : public Declaration {
public:
    StructMember(const std::string &name,
                 antlr4::Token *token,
                 const std::shared_ptr<ty::Type> &type);

    std::vector<Node *> get_children() override;
};

class Struct : public Declaration {
public:
    std::vector<std::shared_ptr<StructMember>> members;

    Struct(const std::string &name,
           antlr4::Token *token,
           const std::vector<std::shared_ptr<StructMember>> &members);

    std::vector<Node *> get_children() override;
};

}
}
}
