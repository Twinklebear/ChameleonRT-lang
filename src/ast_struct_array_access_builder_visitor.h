#include <memory>
#include <vector>
#include "ChameleonRTParserBaseVisitor.h"
#include "antlr4-runtime.h"
#include "ast/expression.h"
#include "ast/type.h"
#include "error_listener.h"

namespace crtl {
/* The ASTStructArrayAccessBuilderVisitor takes a node with some chain of structAccess and/or
 * arrayAccess child nodes and visits them all to construct a vector containing this list of
 * expressions
 */
class ASTStructArrayAccessBuilderVisitor : public crtg::ChameleonRTParserBaseVisitor,
                                           public ErrorReporter {
public:
    std::vector<std::shared_ptr<ast::expr::StructArrayAccessFragment>> struct_array_chain;

    virtual antlrcpp::Any visitStructAccess(
        crtg::ChameleonRTParser::StructAccessContext *ctx) override;

    virtual antlrcpp::Any visitArrayAccess(
        crtg::ChameleonRTParser::ArrayAccessContext *ctx) override;
};

}