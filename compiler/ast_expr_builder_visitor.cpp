#include "ast_expr_builder_visitor.h"
#include "ast_struct_array_access_builder_visitor.h"

namespace crtl {

using namespace ast;

std::any ASTExprBuilderVisitor::visitCall(crtg::ChameleonRTParser::CallContext *ctx)
{
    auto call = std::any_cast<std::shared_ptr<expr::FunctionCall>>(visit(ctx->functionCall()));
    if (ctx->structArrayAccessChain()) {
        ASTStructArrayAccessBuilderVisitor struct_array_access_visitor;
        struct_array_access_visitor.visitChildren(ctx->structArrayAccessChain());
        call->struct_array_access = struct_array_access_visitor.struct_array_chain;
    }
    return std::dynamic_pointer_cast<expr::Expression>(call);
}

std::any ASTExprBuilderVisitor::visitLogicOr(crtg::ChameleonRTParser::LogicOrContext *ctx)
{
    auto lhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(0)));
    auto rhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(1)));
    return std::dynamic_pointer_cast<expr::Expression>(
        expr::Binary::logic_and(ctx->BOOL_OR()->getSymbol(), lhs, rhs));
}

std::any ASTExprBuilderVisitor::visitAddSub(crtg::ChameleonRTParser::AddSubContext *ctx)
{
    // Note/TODO: type checking that this operation is actually valid will be in a later pass
    auto lhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(0)));
    auto rhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(1)));
    std::shared_ptr<expr::Expression> expr;
    if (ctx->PLUS()) {
        expr = expr::Binary::add(ctx->PLUS()->getSymbol(), lhs, rhs);
    } else {
        expr = expr::Binary::subtract(ctx->PLUS()->getSymbol(), lhs, rhs);
    }
    return expr;
}

std::any ASTExprBuilderVisitor::visitParens(crtg::ChameleonRTParser::ParensContext *ctx)
{
    // The parens are only needed to tell the parser how to group things. So we just pass on to
    // the expression in the parenthesis.
    return visit(ctx->expr());
}

std::any ASTExprBuilderVisitor::visitUnary(crtg::ChameleonRTParser::UnaryContext *ctx)
{
    auto operand = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr()));
    std::shared_ptr<expr::Expression> expr;
    if (ctx->MINUS()) {
        expr = expr::Unary::negate(ctx->MINUS()->getSymbol(), operand);
    } else {
        expr = expr::Unary::logic_not(ctx->BANG()->getSymbol(), operand);
    }
    return expr;
}

std::any ASTExprBuilderVisitor::visitStructArray(
    crtg::ChameleonRTParser::StructArrayContext *ctx)
{
    // Visit any struct/array access chain that we may have in the expression
    ASTStructArrayAccessBuilderVisitor struct_array_access_visitor;
    struct_array_access_visitor.visitChildren(ctx->structArrayAccessChain());
    auto var = std::make_shared<expr::Variable>(ctx->IDENTIFIER()->getSymbol());
    auto expr = std::make_shared<expr::StructArrayAccess>(
        var, struct_array_access_visitor.struct_array_chain);

    return std::dynamic_pointer_cast<expr::Expression>(expr);
}

std::any ASTExprBuilderVisitor::visitDiv(crtg::ChameleonRTParser::DivContext *ctx)
{
    // Note/TODO: type checking that this operation is actually valid will be in a later pass
    auto lhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(0)));
    auto rhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(1)));
    return std::dynamic_pointer_cast<expr::Expression>(
        expr::Binary::divide(ctx->SLASH()->getSymbol(), lhs, rhs));
}

std::any ASTExprBuilderVisitor::visitMult(crtg::ChameleonRTParser::MultContext *ctx)
{
    // Note/TODO: type checking that this operation is actually valid will be in a later pass
    auto lhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(0)));
    auto rhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(1)));
    return std::dynamic_pointer_cast<expr::Expression>(
        expr::Binary::multiply(ctx->STAR()->getSymbol(), lhs, rhs));
}

std::any ASTExprBuilderVisitor::visitComparison(
    crtg::ChameleonRTParser::ComparisonContext *ctx)
{
    // Note/TODO: type checking that this operation is actually valid will be in a later pass
    auto lhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(0)));
    auto rhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(1)));
    std::shared_ptr<expr::Expression> expr;
    if (ctx->LESS()) {
        expr = expr::Binary::cmp_less(ctx->LESS()->getSymbol(), lhs, rhs);
    } else if (ctx->LESS_EQUAL()) {
        expr = expr::Binary::cmp_less(ctx->LESS_EQUAL()->getSymbol(), lhs, rhs);
    } else if (ctx->GREATER()) {
        expr = expr::Binary::cmp_less(ctx->GREATER()->getSymbol(), lhs, rhs);
    } else {
        expr = expr::Binary::cmp_less(ctx->GREATER_EQUAL()->getSymbol(), lhs, rhs);
    }
    return expr;
}

std::any ASTExprBuilderVisitor::visitPrimary(crtg::ChameleonRTParser::PrimaryContext *ctx)
{
    std::shared_ptr<expr::Expression> expr;
    if (ctx->IDENTIFIER()) {
        expr = std::make_shared<expr::Variable>(ctx->IDENTIFIER()->getSymbol());
    } else if (ctx->INTEGER_LITERAL()) {
        int value = std::stoi(ctx->INTEGER_LITERAL()->getText());
        expr = std::make_shared<expr::Constant>(ctx->INTEGER_LITERAL()->getSymbol(), value);
    } else if (ctx->FLOAT_LITERAL()) {
        float value = std::stof(ctx->FLOAT_LITERAL()->getText());
        expr = std::make_shared<expr::Constant>(ctx->FLOAT_LITERAL()->getSymbol(), value);
    } else if (ctx->TRUE()) {
        expr = std::make_shared<expr::Constant>(ctx->TRUE()->getSymbol(), true);
    } else if (ctx->FALSE()) {
        expr = std::make_shared<expr::Constant>(ctx->FALSE()->getSymbol(), false);
    }
    return expr;
}

std::any ASTExprBuilderVisitor::visitAssign(crtg::ChameleonRTParser::AssignContext *ctx)
{
    auto value = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr()));
    std::shared_ptr<expr::Expression> lhs;
    if (ctx->structArrayAccessChain()) {
        ASTStructArrayAccessBuilderVisitor struct_array_access_visitor;
        struct_array_access_visitor.visitChildren(ctx->structArrayAccessChain());

        auto var = std::make_shared<expr::Variable>(ctx->IDENTIFIER()->getSymbol());
        lhs = std::make_shared<expr::StructArrayAccess>(
            var, struct_array_access_visitor.struct_array_chain);
    } else {
        lhs = std::make_shared<expr::Variable>(ctx->IDENTIFIER()->getSymbol());
    }
    auto assignment = std::make_shared<expr::Assignment>(lhs, value);
    return std::dynamic_pointer_cast<expr::Expression>(assignment);
}

std::any ASTExprBuilderVisitor::visitEquality(crtg::ChameleonRTParser::EqualityContext *ctx)
{
    auto lhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(0)));
    auto rhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(1)));
    std::shared_ptr<expr::Expression> expr;
    if (ctx->NOT_EQUAL()) {
        expr = expr::Binary::cmp_not_equal(ctx->NOT_EQUAL()->getSymbol(), lhs, rhs);
    } else {
        expr = expr::Binary::cmp_equal(ctx->EQUAL_EQUAL()->getSymbol(), lhs, rhs);
    }
    return expr;
}

std::any ASTExprBuilderVisitor::visitLogicAnd(crtg::ChameleonRTParser::LogicAndContext *ctx)
{
    auto lhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(0)));
    auto rhs = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr(1)));
    return std::dynamic_pointer_cast<expr::Expression>(
        expr::Binary::logic_and(ctx->BOOL_AND()->getSymbol(), lhs, rhs));
}

std::any ASTExprBuilderVisitor::visitFunctionCall(
    crtg::ChameleonRTParser::FunctionCallContext *ctx)
{
    auto callee = ctx->IDENTIFIER()->getSymbol();
    std::vector<std::shared_ptr<expr::Expression>> args;
    if (ctx->arguments()) {
        auto ctx_args = ctx->arguments()->expr();
        for (auto *a : ctx_args) {
            args.push_back(std::any_cast<std::shared_ptr<expr::Expression>>(visit(a)));
        }
    }
    return std::make_shared<expr::FunctionCall>(callee, args);
}

}
