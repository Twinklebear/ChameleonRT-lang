#include "visitor.h"
#include <stdexcept>
#include <vector>

namespace crtl {
namespace ast {

std::any Visitor::visit_ast(AST *ast)
{
    std::vector<std::any> results;
    for (auto &n : ast->top_level_decls) {
        results.push_back(visit(n.get()));
    }
    return results;
}

std::any Visitor::visit(Node *n)
{
    if (!n) {
        throw std::runtime_error("Null node passed to visit!");
    }
    // switch on type and dispatch to visit
    switch (n->get_node_type()) {
    // Declarations
    case NodeType::DECL_FCN:
        return visit_decl_function(dynamic_cast<decl::Function *>(n));
    case NodeType::DECL_ENTRY_POINT:
        return visit_decl_entry_point(dynamic_cast<decl::EntryPoint *>(n));
    case NodeType::DECL_GLOBAL_PARAM:
        return visit_decl_global_param(dynamic_cast<decl::GlobalParam *>(n));
    case NodeType::DECL_STRUCT:
        return visit_decl_struct(dynamic_cast<decl::Struct *>(n));
    case NodeType::DECL_STRUCT_MEMBER:
        return visit_decl_struct_member(dynamic_cast<decl::StructMember *>(n));
    case NodeType::DECL_VAR:
        return visit_decl_variable(dynamic_cast<decl::Variable *>(n));

    // Statements
    case NodeType::STMT_BLOCK:
        return visit_stmt_block(dynamic_cast<stmt::Block *>(n));
    case NodeType::STMT_IF_ELSE:
        return visit_stmt_if_else(dynamic_cast<stmt::IfElse *>(n));
    case NodeType::STMT_WHILE:
        return visit_stmt_while(dynamic_cast<stmt::While *>(n));
        // TODO: do-while isn't in the lexer/parser
        // STMT_DO_WHILE,
    case NodeType::STMT_FOR:
        return visit_stmt_for(dynamic_cast<stmt::For *>(n));
    case NodeType::STMT_RETURN:
        return visit_stmt_return(dynamic_cast<stmt::Return *>(n));
    case NodeType::STMT_EXPR:
        return visit_stmt_expression(dynamic_cast<stmt::Expression *>(n));
    case NodeType::STMT_VAR_DECL:
        return visit_stmt_variable_declaration(dynamic_cast<stmt::VariableDeclaration *>(n));

    // Expressions
    case NodeType::EXPR_NEGATE:
    case NodeType::EXPR_LOGIC_NOT:
        return visit_expr_unary(dynamic_cast<expr::Unary *>(n));
        return visit_expr_unary(dynamic_cast<expr::Unary *>(n));
    case NodeType::EXPR_MULT:
    case NodeType::EXPR_DIV:
    case NodeType::EXPR_ADD:
    case NodeType::EXPR_SUB:
    case NodeType::EXPR_CMP_LESS:
    case NodeType::EXPR_CMP_LESS_EQUAL:
    case NodeType::EXPR_CMP_GREATER:
    case NodeType::EXPR_CMP_GREATER_EQUAL:
    case NodeType::EXPR_CMP_NOT_EQUAL:
    case NodeType::EXPR_CMP_EQUAL:
    case NodeType::EXPR_LOGIC_AND:
    case NodeType::EXPR_LOGIC_OR:
        return visit_expr_binary(dynamic_cast<expr::Binary *>(n));
    case NodeType::EXPR_LITERAL_VAR:
        return visit_expr_variable(dynamic_cast<expr::Variable *>(n));
    case NodeType::EXPR_LITERAL_CONSTANT:
        return visit_expr_constant(dynamic_cast<expr::Constant *>(n));
    case NodeType::EXPR_FCN_CALL:
        return visit_expr_function_call(dynamic_cast<expr::FunctionCall *>(n));
    case NodeType::EXPR_STRUCT_ARRAY_ACCESS:
        return visit_struct_array_access(dynamic_cast<expr::StructArrayAccess *>(n));
    case NodeType::EXPR_ASSIGN:
        return visit_expr_assignment(dynamic_cast<expr::Assignment *>(n));
    default:
        throw std::runtime_error("Invalid node type encountered in visitor!");
        break;
    }
    return std::any();
}

std::any Visitor::visit_children(Node *n)
{
    auto children = n->get_children();
    std::vector<std::any> child_results;
    for (auto *c : children) {
        child_results.push_back(visit(c));
    }
    return child_results;
}

std::any Visitor::visit_decl_function(decl::Function *d)
{
    return visit_children(d);
}

std::any Visitor::visit_decl_entry_point(decl::EntryPoint *d)
{
    return visit_children(d);
}

std::any Visitor::visit_decl_global_param(decl::GlobalParam *d)
{
    return visit_children(d);
}

std::any Visitor::visit_decl_struct(decl::Struct *d)
{
    return visit_children(d);
}

std::any Visitor::visit_decl_struct_member(decl::StructMember *d)
{
    return visit_children(d);
}

std::any Visitor::visit_decl_variable(decl::Variable *d)
{
    return visit_children(d);
}

std::any Visitor::visit_stmt_block(stmt::Block *s)
{
    return visit_children(s);
}

std::any Visitor::visit_stmt_if_else(stmt::IfElse *s)
{
    return visit_children(s);
}

std::any Visitor::visit_stmt_while(stmt::While *s)
{
    return visit_children(s);
}

std::any Visitor::visit_stmt_for(stmt::For *s)
{
    return visit_children(s);
}

std::any Visitor::visit_stmt_return(stmt::Return *s)
{
    return visit_children(s);
}

std::any Visitor::visit_stmt_variable_declaration(stmt::VariableDeclaration *s)
{
    return visit_children(s);
}

std::any Visitor::visit_stmt_expression(stmt::Expression *s)
{
    return visit_children(s);
}

std::any Visitor::visit_expr_unary(expr::Unary *e)
{
    return visit_children(e);
}

std::any Visitor::visit_expr_binary(expr::Binary *e)
{
    return visit_children(e);
}

std::any Visitor::visit_expr_variable(expr::Variable *e)
{
    return visit_children(e);
}

std::any Visitor::visit_expr_constant(expr::Constant *e)
{
    return visit_children(e);
}

std::any Visitor::visit_expr_function_call(expr::FunctionCall *e)
{
    return visit_children(e);
}

std::any Visitor::visit_struct_array_access(expr::StructArrayAccess *e)
{
    return visit_children(e);
}

std::any Visitor::visit_expr_assignment(expr::Assignment *e)
{
    return visit_children(e);
}

}
}
