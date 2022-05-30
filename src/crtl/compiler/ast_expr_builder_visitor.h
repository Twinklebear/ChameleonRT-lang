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
 * is returned in the std::any as a std::shared_ptr<ast::expr::Expression>
 */
class ASTExprBuilderVisitor : public crtg::ChameleonRTParserBaseVisitor, public ErrorReporter {
public:
    virtual std::any visitCall(crtg::ChameleonRTParser::CallContext *ctx) override;

    virtual std::any visitLogicOr(crtg::ChameleonRTParser::LogicOrContext *ctx) override;

    virtual std::any visitAddSub(crtg::ChameleonRTParser::AddSubContext *ctx) override;

    virtual std::any visitParens(crtg::ChameleonRTParser::ParensContext *ctx) override;

    virtual std::any visitUnary(crtg::ChameleonRTParser::UnaryContext *ctx) override;

    virtual std::any visitStructArray(
        crtg::ChameleonRTParser::StructArrayContext *ctx) override;

    virtual std::any visitDiv(crtg::ChameleonRTParser::DivContext *ctx) override;

    virtual std::any visitMult(crtg::ChameleonRTParser::MultContext *ctx) override;

    virtual std::any visitComparison(crtg::ChameleonRTParser::ComparisonContext *ctx) override;

    virtual std::any visitPrimary(crtg::ChameleonRTParser::PrimaryContext *ctx) override;

    virtual std::any visitAssign(crtg::ChameleonRTParser::AssignContext *ctx) override;

    virtual std::any visitEquality(crtg::ChameleonRTParser::EqualityContext *ctx) override;

    virtual std::any visitLogicAnd(crtg::ChameleonRTParser::LogicAndContext *ctx) override;

    virtual std::any visitFunctionCall(
        crtg::ChameleonRTParser::FunctionCallContext *ctx) override;
};

}
