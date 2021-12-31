#include "ast_expr_builder_visitor.h"
#include "ast_struct_array_access_builder_visitor.h"

namespace crtl {

using namespace ast;

antlrcpp::Any ASTExprBuilderVisitor::visitCall(crtg::ChameleonRTParser::CallContext *ctx)
{
    auto call = visit(ctx->functionCall()).as<std::shared_ptr<expr::FunctionCall>>();
    if (ctx->structArrayAccessChain()) {
        ASTStructArrayAccessBuilderVisitor struct_array_access_visitor;
        struct_array_access_visitor.visitChildren(ctx->structArrayAccessChain());
        call->struct_array_access = struct_array_access_visitor.struct_array_chain;
    }
    return std::dynamic_pointer_cast<expr::Expression>(call);
}

antlrcpp::Any ASTExprBuilderVisitor::visitLogicOr(crtg::ChameleonRTParser::LogicOrContext *ctx)
{
    auto lhs = visit(ctx->expr(0)).as<std::shared_ptr<expr::Expression>>();
    auto rhs = visit(ctx->expr(1)).as<std::shared_ptr<expr::Expression>>();
    return std::dynamic_pointer_cast<expr::Expression>(
        expr::Binary::logic_and(ctx->BOOL_OR()->getSymbol(), lhs, rhs));
}

antlrcpp::Any ASTExprBuilderVisitor::visitAddSub(crtg::ChameleonRTParser::AddSubContext *ctx)
{
    // Note/TODO: type checking that this operation is actually valid will be in a later pass
    auto lhs = visit(ctx->expr(0)).as<std::shared_ptr<expr::Expression>>();
    auto rhs = visit(ctx->expr(1)).as<std::shared_ptr<expr::Expression>>();
    std::shared_ptr<expr::Expression> expr;
    if (ctx->PLUS()) {
        expr = expr::Binary::add(ctx->PLUS()->getSymbol(), lhs, rhs);
    } else {
        expr = expr::Binary::subtract(ctx->PLUS()->getSymbol(), lhs, rhs);
    }
    return expr;
}

antlrcpp::Any ASTExprBuilderVisitor::visitParens(crtg::ChameleonRTParser::ParensContext *ctx)
{
    // The parens are only needed to tell the parser how to group things. So we just pass on to
    // the expression in the parenthesis.
    return visit(ctx->expr());
}

antlrcpp::Any ASTExprBuilderVisitor::visitUnary(crtg::ChameleonRTParser::UnaryContext *ctx)
{
    auto operand = visit(ctx->expr()).as<std::shared_ptr<expr::Expression>>();
    std::shared_ptr<expr::Expression> expr;
    if (ctx->MINUS()) {
        expr = expr::Unary::negate(ctx->MINUS()->getSymbol(), operand);
    } else {
        expr = expr::Unary::logic_not(ctx->BANG()->getSymbol(), operand);
    }
    return expr;
}

antlrcpp::Any ASTExprBuilderVisitor::visitStructArray(
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

antlrcpp::Any ASTExprBuilderVisitor::visitDiv(crtg::ChameleonRTParser::DivContext *ctx)
{
    // Note/TODO: type checking that this operation is actually valid will be in a later pass
    auto lhs = visit(ctx->expr(0)).as<std::shared_ptr<expr::Expression>>();
    auto rhs = visit(ctx->expr(1)).as<std::shared_ptr<expr::Expression>>();
    return std::dynamic_pointer_cast<expr::Expression>(
        expr::Binary::divide(ctx->SLASH()->getSymbol(), lhs, rhs));
}

antlrcpp::Any ASTExprBuilderVisitor::visitMult(crtg::ChameleonRTParser::MultContext *ctx)
{
    // Note/TODO: type checking that this operation is actually valid will be in a later pass
    auto lhs = visit(ctx->expr(0)).as<std::shared_ptr<expr::Expression>>();
    auto rhs = visit(ctx->expr(1)).as<std::shared_ptr<expr::Expression>>();
    return std::dynamic_pointer_cast<expr::Expression>(
        expr::Binary::multiply(ctx->STAR()->getSymbol(), lhs, rhs));
}

antlrcpp::Any ASTExprBuilderVisitor::visitComparison(
    crtg::ChameleonRTParser::ComparisonContext *ctx)
{
    // Note/TODO: type checking that this operation is actually valid will be in a later pass
    auto lhs = visit(ctx->expr(0)).as<std::shared_ptr<expr::Expression>>();
    auto rhs = visit(ctx->expr(1)).as<std::shared_ptr<expr::Expression>>();
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

antlrcpp::Any ASTExprBuilderVisitor::visitPrimary(crtg::ChameleonRTParser::PrimaryContext *ctx)
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

antlrcpp::Any ASTExprBuilderVisitor::visitAssign(crtg::ChameleonRTParser::AssignContext *ctx)
{
    auto expr = visit(ctx->expr()).as<std::shared_ptr<expr::Expression>>();
    auto assignment = std::make_shared<expr::Assignment>(ctx->IDENTIFIER()->getSymbol(), expr);
    if (ctx->structArrayAccessChain()) {
        ASTStructArrayAccessBuilderVisitor struct_array_access_visitor;
        struct_array_access_visitor.visitChildren(ctx->structArrayAccessChain());
        assignment->struct_array_access = struct_array_access_visitor.struct_array_chain;
    }
    return std::dynamic_pointer_cast<expr::Expression>(assignment);
}

antlrcpp::Any ASTExprBuilderVisitor::visitEquality(
    crtg::ChameleonRTParser::EqualityContext *ctx)
{
    auto lhs = visit(ctx->expr(0)).as<std::shared_ptr<expr::Expression>>();
    auto rhs = visit(ctx->expr(1)).as<std::shared_ptr<expr::Expression>>();
    std::shared_ptr<expr::Expression> expr;
    if (ctx->NOT_EQUAL()) {
        expr = expr::Binary::cmp_not_equal(ctx->NOT_EQUAL()->getSymbol(), lhs, rhs);
    } else {
        expr = expr::Binary::cmp_equal(ctx->EQUAL_EQUAL()->getSymbol(), lhs, rhs);
    }
    return expr;
}

antlrcpp::Any ASTExprBuilderVisitor::visitLogicAnd(
    crtg::ChameleonRTParser::LogicAndContext *ctx)
{
    auto lhs = visit(ctx->expr(0)).as<std::shared_ptr<expr::Expression>>();
    auto rhs = visit(ctx->expr(1)).as<std::shared_ptr<expr::Expression>>();
    return std::dynamic_pointer_cast<expr::Expression>(
        expr::Binary::logic_and(ctx->BOOL_AND()->getSymbol(), lhs, rhs));
}

antlrcpp::Any ASTExprBuilderVisitor::visitFunctionCall(
    crtg::ChameleonRTParser::FunctionCallContext *ctx)
{
    auto callee = ctx->IDENTIFIER()->getSymbol();
    std::vector<std::shared_ptr<expr::Expression>> args;
    if (ctx->arguments()) {
        auto ctx_args = ctx->arguments()->expr();
        for (auto *a : ctx_args) {
            args.push_back(visit(a).as<std::shared_ptr<expr::Expression>>());
        }
    }
    return std::make_shared<expr::FunctionCall>(callee, args);
}

}
