#include "node.h"

namespace crtl {
namespace ast {

std::string to_string(const NodeType &nt)
{
    switch (nt) {
    case NodeType::INVALID:
        return "INVALID";
    case NodeType::DECL_FCN:
        return "DECL_FCN";
    case NodeType::DECL_ENTRY_POINT:
        return "DECL_ENTRY_POINT";
    case NodeType::DECL_GLOBAL_PARAM:
        return "DECL_GLOBAL_PARAM";
    case NodeType::DECL_STRUCT:
        return "DECL_STRUCT";
    case NodeType::DECL_STRUCT_MEMBER:
        return "DECL_STRUCT_MEMBER";
    case NodeType::DECL_VAR:
        return "DECL_VAR";
    case NodeType::STMT_BLOCK:
        return "STMT_BLOCK";
    case NodeType::STMT_IF_ELSE:
        return "STMT_IF_ELSE";
    case NodeType::STMT_WHILE:
        return "STMT_WHILE";
    case NodeType::STMT_FOR:
        return "STMT_FOR";
    case NodeType::STMT_RETURN:
        return "STMT_RETURN";
    case NodeType::STMT_EXPR:
        return "STMT_EXPR";
    case NodeType::STMT_VAR_DECL:
        return "STMT_VAR_DECL";
    case NodeType::EXPR_NEGATE:
        return "EXPR_NEGATE";
    case NodeType::EXPR_LOGIC_NOT:
        return "EXPR_LOGIC_NOT";
    case NodeType::EXPR_MULT:
        return "EXPR_MULT";
    case NodeType::EXPR_DIV:
        return "EXPR_DIV";
    case NodeType::EXPR_ADD:
        return "EXPR_ADD";
    case NodeType::EXPR_SUB:
        return "EXPR_SUB";
    case NodeType::EXPR_CMP_LESS:
        return "EXPR_CMP_LESS";
    case NodeType::EXPR_CMP_LESS_EQUAL:
        return "EXPR_CMP_LESS_EQUAL";
    case NodeType::EXPR_CMP_GREATER:
        return "EXPR_CMP_GREATER";
    case NodeType::EXPR_CMP_GREATER_EQUAL:
        return "EXPR_CMP_GREATER_EQUAL";
    case NodeType::EXPR_CMP_NOT_EQUAL:
        return "EXPR_CMP_NOT_EQUAL";
    case NodeType::EXPR_CMP_EQUAL:
        return "EXPR_CMP_EQUAL";
    case NodeType::EXPR_LOGIC_AND:
        return "EXPR_LOGIC_AND";
    case NodeType::EXPR_LOGIC_OR:
        return "EXPR_LOGIC_OR";
    case NodeType::EXPR_LITERAL_VAR:
        return "EXPR_LITERAL_VAR";
    case NodeType::EXPR_LITERAL_CONSTANT:
        return "EXPR_LITERAL_CONSTANT";
    case NodeType::EXPR_FCN_CALL:
        return "EXPR_FCN_CALL";
    case NodeType::EXPR_STRUCT_ARRAY_ACCESS:
        return "EXPR_STRUCT_ARRAY_ACCESS";
    case NodeType::EXPR_ASSIGN:
        return "EXPR_ASSIGN";
    default:
        return "INVALID";
    }
}

Node::Node(antlr4::Token *token, NodeType type) : token(token), node_type(type) {}

NodeType Node::get_node_type() const
{
    return node_type;
}

antlr4::Token *Node::get_token()
{
    return token;
}

const antlr4::Token *Node::get_token() const
{
    return token;
}

}
}
