#pragma once

#include <memory>
#include <set>
#include <vector>
#include "ChameleonRTParserBaseVisitor.h"
#include "antlr4-runtime.h"
#include "ast/node.h"
#include "ast/type.h"
#include "error_listener.h"

namespace crtl {
class ASTBuilderVisitor : public crtg::ChameleonRTParserBaseVisitor, public ErrorReporter {
public:
    std::shared_ptr<ast::AST> ast = std::make_shared<ast::AST>();

    virtual antlrcpp::Any visitTopLevelDeclaration(
        crtg::ChameleonRTParser::TopLevelDeclarationContext *ctx) override;

    virtual antlrcpp::Any visitFunctionDecl(
        crtg::ChameleonRTParser::FunctionDeclContext *ctx) override;

    virtual antlrcpp::Any visitStructDecl(
        crtg::ChameleonRTParser::StructDeclContext *ctx) override;

    virtual antlrcpp::Any visitStructMember(
        crtg::ChameleonRTParser::StructMemberContext *ctx) override;

    virtual antlrcpp::Any visitParameterList(
        crtg::ChameleonRTParser::ParameterListContext *ctx) override;

    virtual antlrcpp::Any visitParameter(
        crtg::ChameleonRTParser::ParameterContext *ctx) override;

    virtual antlrcpp::Any visitBlock(crtg::ChameleonRTParser::BlockContext *ctx) override;

    virtual antlrcpp::Any visitVarDecl(crtg::ChameleonRTParser::VarDeclContext *ctx) override;

    virtual antlrcpp::Any visitVarDeclStmt(
        crtg::ChameleonRTParser::VarDeclStmtContext *ctx) override;

    virtual antlrcpp::Any visitGlobalParamDecl(
        crtg::ChameleonRTParser::GlobalParamDeclContext *ctx) override;

    virtual antlrcpp::Any visitIfStmt(crtg::ChameleonRTParser::IfStmtContext *ctx) override;

    virtual antlrcpp::Any visitWhileStmt(
        crtg::ChameleonRTParser::WhileStmtContext *ctx) override;

    virtual antlrcpp::Any visitForStmt(crtg::ChameleonRTParser::ForStmtContext *ctx) override;

    virtual antlrcpp::Any visitReturnStmt(
        crtg::ChameleonRTParser::ReturnStmtContext *ctx) override;

    virtual antlrcpp::Any visitExprStmt(
        crtg::ChameleonRTParser::ExprStmtContext *ctx) override;

    virtual antlrcpp::Any visitTypeName(
        crtg::ChameleonRTParser::TypeNameContext *ctx) override;

    virtual antlrcpp::Any visitTemplateParameters(
        crtg::ChameleonRTParser::TemplateParametersContext *ctx) override;

    std::set<ast::ty::Modifier> parse_modifiers(
        antlr4::Token *token,
        const std::vector<crtg::ChameleonRTParser::ModifierContext *> &modifier_list);

    // Expression visitors are overriden for convenience, but all forward on to the
    // ASTExprBuilderVisitor
    virtual antlrcpp::Any visitUnary(crtg::ChameleonRTParser::UnaryContext *ctx) override;
    virtual antlrcpp::Any visitCall(crtg::ChameleonRTParser::CallContext *ctx) override;
    virtual antlrcpp::Any visitStructArray(
        crtg::ChameleonRTParser::StructArrayContext *ctx) override;
    virtual antlrcpp::Any visitMult(crtg::ChameleonRTParser::MultContext *ctx) override;
    virtual antlrcpp::Any visitDiv(crtg::ChameleonRTParser::DivContext *ctx) override;
    virtual antlrcpp::Any visitAddSub(crtg::ChameleonRTParser::AddSubContext *ctx) override;
    virtual antlrcpp::Any visitComparison(
        crtg::ChameleonRTParser::ComparisonContext *ctx) override;
    virtual antlrcpp::Any visitEquality(
        crtg::ChameleonRTParser::EqualityContext *ctx) override;
    virtual antlrcpp::Any visitLogicAnd(
        crtg::ChameleonRTParser::LogicAndContext *ctx) override;
    virtual antlrcpp::Any visitLogicOr(crtg::ChameleonRTParser::LogicOrContext *ctx) override;
    virtual antlrcpp::Any visitAssign(crtg::ChameleonRTParser::AssignContext *ctx) override;
    virtual antlrcpp::Any visitParens(crtg::ChameleonRTParser::ParensContext *ctx) override;
    virtual antlrcpp::Any visitPrimary(crtg::ChameleonRTParser::PrimaryContext *ctx) override;

    antlrcpp::Any visit_expr(crtg::ChameleonRTParser::ExprContext *ctx);
};
}
