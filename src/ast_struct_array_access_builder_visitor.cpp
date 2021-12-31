#include "ast_struct_array_access_builder_visitor.h"
#include "ast_expr_builder_visitor.h"

namespace crtl {

using namespace ast;

antlrcpp::Any ASTStructArrayAccessBuilderVisitor::visitStructAccess(
    crtg::ChameleonRTParser::StructAccessContext *ctx)
{
    struct_array_chain.push_back(
        std::make_shared<expr::StructMemberAccessFragment>(ctx->IDENTIFIER()->getSymbol()));
    return antlrcpp::Any();
}

antlrcpp::Any ASTStructArrayAccessBuilderVisitor::visitArrayAccess(
    crtg::ChameleonRTParser::ArrayAccessContext *ctx)
{
    ASTExprBuilderVisitor expr_visitor;
    auto index = expr_visitor.visit(ctx->expr()).as<std::shared_ptr<expr::Expression>>();
    struct_array_chain.push_back(std::make_shared<expr::ArrayAccessFragment>(index));
    return antlrcpp::Any();
}

}