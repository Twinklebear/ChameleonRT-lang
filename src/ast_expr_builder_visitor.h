#pragma once

#include <memory>
#include <set>
#include <vector>
#include "ChameleonRTParserBaseVisitor.h"
#include "antlr4-runtime.h"
#include "ast/expression.h"
#include "ast/type.h"
#include "error_listener.h"

namespace crtl {

/* The ASTExprBuilderVisitor visits the passed expression and constructs the AST expression
 * node corresponding to the expression and any subexpressions. The root of the expression tree
 * is returned in the antlrcpp::Any as a std::shared_ptr<ast::expr::Expression>
 */
class ASTExprBuilderVisitor : public crtg::ChameleonRTParserBaseVisitor, public ErrorReporter {
public:
    virtual antlrcpp::Any visitCall(crtg::ChameleonRTParser::CallContext *ctx) override;

    virtual antlrcpp::Any visitLogicOr(crtg::ChameleonRTParser::LogicOrContext *ctx) override;

    virtual antlrcpp::Any visitAddSub(crtg::ChameleonRTParser::AddSubContext *ctx) override;

    virtual antlrcpp::Any visitParens(crtg::ChameleonRTParser::ParensContext *ctx) override;

    virtual antlrcpp::Any visitUnary(crtg::ChameleonRTParser::UnaryContext *ctx) override;

    virtual antlrcpp::Any visitStructArray(
        crtg::ChameleonRTParser::StructArrayContext *ctx) override;

    virtual antlrcpp::Any visitDiv(crtg::ChameleonRTParser::DivContext *ctx) override;

    virtual antlrcpp::Any visitMult(crtg::ChameleonRTParser::MultContext *ctx) override;

    virtual antlrcpp::Any visitComparison(
        crtg::ChameleonRTParser::ComparisonContext *ctx) override;

    virtual antlrcpp::Any visitPrimary(crtg::ChameleonRTParser::PrimaryContext *ctx) override;

    virtual antlrcpp::Any visitAssign(crtg::ChameleonRTParser::AssignContext *ctx) override;

    virtual antlrcpp::Any visitEquality(
        crtg::ChameleonRTParser::EqualityContext *ctx) override;

    virtual antlrcpp::Any visitLogicAnd(
        crtg::ChameleonRTParser::LogicAndContext *ctx) override;

    virtual antlrcpp::Any visitFunctionCall(
        crtg::ChameleonRTParser::FunctionCallContext *ctx) override;
};

}