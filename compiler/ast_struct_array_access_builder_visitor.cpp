#include "ast_struct_array_access_builder_visitor.h"
#include "ast_expr_builder_visitor.h"

namespace crtl {

using namespace ast;

std::any ASTStructArrayAccessBuilderVisitor::visitStructAccess(
    crtg::ChameleonRTParser::StructAccessContext *ctx)
{
    struct_array_chain.push_back(
        std::make_shared<expr::StructMemberAccessFragment>(ctx->IDENTIFIER()->getSymbol()));
    return std::any();
}

std::any ASTStructArrayAccessBuilderVisitor::visitArrayAccess(
    crtg::ChameleonRTParser::ArrayAccessContext *ctx)
{
    ASTExprBuilderVisitor expr_visitor;
    auto index =
        std::any_cast<std::shared_ptr<expr::Expression>>(expr_visitor.visit(ctx->expr()));
    struct_array_chain.push_back(std::make_shared<expr::ArrayAccessFragment>(index));
    return std::any();
}

}
