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

    virtual std::any visitTopLevelDeclaration(
        crtg::ChameleonRTParser::TopLevelDeclarationContext *ctx) override;

    virtual std::any visitFunctionDecl(
        crtg::ChameleonRTParser::FunctionDeclContext *ctx) override;

    virtual std::any visitStructDecl(crtg::ChameleonRTParser::StructDeclContext *ctx) override;

    virtual std::any visitStructMember(
        crtg::ChameleonRTParser::StructMemberContext *ctx) override;

    virtual std::any visitParameterList(
        crtg::ChameleonRTParser::ParameterListContext *ctx) override;

    virtual std::any visitParameter(crtg::ChameleonRTParser::ParameterContext *ctx) override;

    virtual std::any visitBlock(crtg::ChameleonRTParser::BlockContext *ctx) override;

    virtual std::any visitVarDecl(crtg::ChameleonRTParser::VarDeclContext *ctx) override;

    virtual std::any visitVarDeclStmt(
        crtg::ChameleonRTParser::VarDeclStmtContext *ctx) override;

    virtual std::any visitGlobalParamDecl(
        crtg::ChameleonRTParser::GlobalParamDeclContext *ctx) override;

    virtual std::any visitIfStmt(crtg::ChameleonRTParser::IfStmtContext *ctx) override;

    virtual std::any visitWhileStmt(crtg::ChameleonRTParser::WhileStmtContext *ctx) override;

    virtual std::any visitForStmt(crtg::ChameleonRTParser::ForStmtContext *ctx) override;

    virtual std::any visitReturnStmt(crtg::ChameleonRTParser::ReturnStmtContext *ctx) override;

    virtual std::any visitExprStmt(crtg::ChameleonRTParser::ExprStmtContext *ctx) override;

    virtual std::any visitTypeName(crtg::ChameleonRTParser::TypeNameContext *ctx) override;

    virtual std::any visitTemplateParameters(
        crtg::ChameleonRTParser::TemplateParametersContext *ctx) override;

    std::set<ast::ty::Modifier> parse_modifiers(
        antlr4::Token *token,
        const std::vector<crtg::ChameleonRTParser::ModifierContext *> &modifier_list);

    // Expression visitors are overriden for convenience, but all forward on to the
    // ASTExprBuilderVisitor
    virtual std::any visitUnary(crtg::ChameleonRTParser::UnaryContext *ctx) override;
    virtual std::any visitCall(crtg::ChameleonRTParser::CallContext *ctx) override;
    virtual std::any visitStructArray(
        crtg::ChameleonRTParser::StructArrayContext *ctx) override;
    virtual std::any visitMult(crtg::ChameleonRTParser::MultContext *ctx) override;
    virtual std::any visitDiv(crtg::ChameleonRTParser::DivContext *ctx) override;
    virtual std::any visitAddSub(crtg::ChameleonRTParser::AddSubContext *ctx) override;
    virtual std::any visitComparison(crtg::ChameleonRTParser::ComparisonContext *ctx) override;
    virtual std::any visitEquality(crtg::ChameleonRTParser::EqualityContext *ctx) override;
    virtual std::any visitLogicAnd(crtg::ChameleonRTParser::LogicAndContext *ctx) override;
    virtual std::any visitLogicOr(crtg::ChameleonRTParser::LogicOrContext *ctx) override;
    virtual std::any visitAssign(crtg::ChameleonRTParser::AssignContext *ctx) override;
    virtual std::any visitParens(crtg::ChameleonRTParser::ParensContext *ctx) override;
    virtual std::any visitPrimary(crtg::ChameleonRTParser::PrimaryContext *ctx) override;

    std::any visit_expr(crtg::ChameleonRTParser::ExprContext *ctx);
};
}
