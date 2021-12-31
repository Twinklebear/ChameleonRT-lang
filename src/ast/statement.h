#pragma once

#include "declaration.h"
#include "expression.h"
#include "node.h"

namespace crtl {
namespace ast {
namespace stmt {

class Statement : public Node {
public:
    Statement(antlr4::Token *token, NodeType stmt_type);
};

class Block : public Statement {
public:
    std::vector<std::shared_ptr<Statement>> statements;

    Block(antlr4::Token *token, const std::vector<std::shared_ptr<Statement>> &statements);

    std::vector<Node *> get_children() override;
};

class IfElse : public Statement {
public:
    std::shared_ptr<expr::Expression> condition;
    std::shared_ptr<Statement> if_branch;
    std::shared_ptr<Statement> else_branch;

    IfElse(antlr4::Token *token,
           const std::shared_ptr<expr::Expression> &condition,
           const std::shared_ptr<Statement> &if_branch,
           const std::shared_ptr<Statement> &else_branch);

    std::vector<Node *> get_children() override;
};

class While : public Statement {
public:
    std::shared_ptr<expr::Expression> condition;
    std::shared_ptr<Statement> body;

    While(antlr4::Token *token,
          const std::shared_ptr<expr::Expression> &condition,
          const std::shared_ptr<Statement> &body);

    std::vector<Node *> get_children() override;
};

class For : public Statement {
public:
    std::shared_ptr<Statement> init;
    std::shared_ptr<expr::Expression> condition;
    std::shared_ptr<expr::Expression> advance;
    std::shared_ptr<Statement> body;

    For(antlr4::Token *token,
        const std::shared_ptr<Statement> &init,
        const std::shared_ptr<expr::Expression> &condition,
        const std::shared_ptr<expr::Expression> &advance,
        const std::shared_ptr<Statement> &body);

    std::vector<Node *> get_children() override;
};

class Return : public Statement {
public:
    std::shared_ptr<expr::Expression> expression;

    Return(antlr4::Token *token, const std::shared_ptr<expr::Expression> &expr);

    std::vector<Node *> get_children() override;
};

class VariableDeclaration : public Statement {
public:
    std::shared_ptr<decl::Variable> var_decl;

    VariableDeclaration(antlr4::Token *token, const std::shared_ptr<decl::Variable> &var_decl);

    std::vector<Node *> get_children() override;
};

class Expression : public Statement {
public:
    std::shared_ptr<expr::Expression> expr;

    Expression(antlr4::Token *token, const std::shared_ptr<expr::Expression> &expr);

    std::vector<Node *> get_children() override;
};

}
}
}
