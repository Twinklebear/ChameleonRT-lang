#include "modifying_visitor.h"

namespace crtl {
namespace ast {

// TODO

std::any ModifyingVisitor::visit_ast(const std::shared_ptr<AST> &ast)
{
    auto ast_out = std::make_shared<AST>();
    for (auto &n : ast->top_level_decls) {
        auto result = visit(n);
        if (!result.has_value()) {
            continue;
        }
        if (result.type() == typeid(std::shared_ptr<decl::Declaration>)) {
            auto decl = std::any_cast<std::shared_ptr<decl::Declaration>>(result);
            ast_out->top_level_decls.push_back(decl);
        } else if (result.type() == typeid(std::vector<std::shared_ptr<decl::Declaration>>)) {
            auto decls =
                std::any_cast<std::vector<std::shared_ptr<decl::Declaration>>>(result);
            for (auto &d : decls) {
                ast_out->top_level_decls.push_back(d);
            }
        } else {
            const std::string type_name = result.type().name();
            std::cout << "ModifyingVisitor: Top-level decl returned invalid type '"
                      << type_name << "'\n";
            throw std::runtime_error(
                "ModifyingVisitor: Top-level decl returned invalid type '" + type_name + "'");
        }
    }
    return ast_out;
}

std::any ModifyingVisitor::visit_decl_function(const std::shared_ptr<decl::Function> &d) {}

std::any ModifyingVisitor::visit_decl_entry_point(const std::shared_ptr<decl::EntryPoint> &d)
{
}

std::any ModifyingVisitor::visit_decl_global_param(const std::shared_ptr<decl::GlobalParam> &d)
{
}

std::any ModifyingVisitor::visit_decl_struct(const std::shared_ptr<decl::Struct> &d) {}

std::any ModifyingVisitor::visit_decl_struct_member(
    const std::shared_ptr<decl::StructMember> &d)
{
}

std::any ModifyingVisitor::visit_decl_variable(const std::shared_ptr<decl::Variable> &d) {}

std::any ModifyingVisitor::visit_stmt_block(const std::shared_ptr<stmt::Block> &s)
{
    // TODO: Need similar handling here as the AST for getting back zero, one, or multiple
    // statements
}

std::any ModifyingVisitor::visit_stmt_if_else(const std::shared_ptr<stmt::IfElse> &s) {}

std::any ModifyingVisitor::visit_stmt_while(const std::shared_ptr<stmt::While> &s) {}

std::any ModifyingVisitor::visit_stmt_for(const std::shared_ptr<stmt::For> &s) {}

std::any ModifyingVisitor::visit_stmt_return(const std::shared_ptr<stmt::Return> &s) {}

std::any ModifyingVisitor::visit_stmt_variable_declaration(
    const std::shared_ptr<stmt::VariableDeclaration> &s)
{
}

std::any ModifyingVisitor::visit_stmt_expression(const std::shared_ptr<stmt::Expression> &s) {}

std::any ModifyingVisitor::visit_expr_unary(const std::shared_ptr<expr::Unary> &e) {}

std::any ModifyingVisitor::visit_expr_binary(const std::shared_ptr<expr::Binary> &e) {}

std::any ModifyingVisitor::visit_expr_variable(const std::shared_ptr<expr::Variable> &e) {}

std::any ModifyingVisitor::visit_expr_constant(const std::shared_ptr<expr::Constant> &e) {}

std::any ModifyingVisitor::visit_expr_function_call(
    const std::shared_ptr<expr::FunctionCall> &e)
{
}

std::any ModifyingVisitor::visit_struct_array_access(
    const std::shared_ptr<expr::StructArrayAccess> &e)
{
}

std::any ModifyingVisitor::visit_expr_assignment(const std::shared_ptr<expr::Assignment> &e) {}
}
}
