#pragma once

#include "ChameleonRTParser.h"
#include "antlr4-common.h"
#include "json.hpp"

namespace crtl {
namespace ast {

enum class NodeType {
    INVALID,

    // Declarations
    DECL_FCN,
    DECL_ENTRY_POINT,
    DECL_GLOBAL_PARAM,
    DECL_STRUCT,
    DECL_STRUCT_MEMBER,
    DECL_VAR,

    // Statements
    STMT_BLOCK,
    STMT_IF_ELSE,
    STMT_WHILE,
    // TODO: do-while isn't in the lexer/parser
    // STMT_DO_WHILE,
    STMT_FOR,
    STMT_RETURN,
    STMT_EXPR,
    STMT_VAR_DECL,

    // Expressions
    // Unary expressions
    EXPR_NEGATE,
    EXPR_LOGIC_NOT,

    // Binary expressions
    EXPR_MULT,
    EXPR_DIV,
    EXPR_ADD,
    EXPR_SUB,
    EXPR_CMP_LESS,
    EXPR_CMP_LESS_EQUAL,
    EXPR_CMP_GREATER,
    EXPR_CMP_GREATER_EQUAL,
    EXPR_CMP_NOT_EQUAL,
    EXPR_CMP_EQUAL,
    EXPR_LOGIC_AND,
    EXPR_LOGIC_OR,

    // Literal expressions (vars, constants)
    EXPR_LITERAL_VAR,
    EXPR_LITERAL_CONSTANT,

    // Other expressions
    EXPR_FCN_CALL,
    EXPR_STRUCT_ARRAY_ACCESS,
    EXPR_ASSIGN,
};

std::string to_string(const NodeType &nt);

/* Node in the AST that is tied to a specific location in the source code
 */
class Node {
protected:
    antlr4::Token *token = nullptr;
    NodeType node_type = NodeType::INVALID;

public:
    Node() = default;

    Node(antlr4::Token *token, NodeType type);

    virtual ~Node() = default;

    NodeType get_node_type() const;

    antlr4::Token *get_token();

    const antlr4::Token *get_token() const;

    virtual std::vector<Node *> get_children() = 0;
};

class AST {
public:
    // The top level declarations in the program
    std::vector<std::shared_ptr<Node>> top_level_decls;
};
}
}
