#pragma once

#include <any>
#include <memory.h>
#include "visitor.h"

namespace crtl {
namespace ast {

/* The ModifyingVisitor's default visit methods will replace the child node being
 * visited with the result returned by visiting it, allowing nodes of the AST to be easily
 * replaced
 */
class ModifyingVisitor : public Visitor {
public:
    virtual ~ModifyingVisitor() override = default;

    /* visit_ast can replace statements with new single nodes or with sets of statements.
     * Visiting a top-level declaration can return a single shared_ptr to a top level
     * declaration to replace the original node with or a vector<shared_ptr> to replace the
     * original node with a set of declarations
     */
    virtual std::any visit_ast(const std::shared_ptr<AST> &ast) override;

    /* Declarations
     *
     * Declarations can be removed by returning an empty std::any, preserved by returning the
     * declaration, or expanded by returning a std::vector of declarations. All declarations
     * returned must be cast up to the parent type as a std::shared_ptr<decl::Declaration>
     */
    virtual std::any visit_decl_function(const std::shared_ptr<decl::Function> &d) override;
    virtual std::any visit_decl_entry_point(
        const std::shared_ptr<decl::EntryPoint> &d) override;
    virtual std::any visit_decl_global_param(
        const std::shared_ptr<decl::GlobalParam> &d) override;
    virtual std::any visit_decl_struct(const std::shared_ptr<decl::Struct> &d) override;
    virtual std::any visit_decl_struct_member(
        const std::shared_ptr<decl::StructMember> &d) override;
    virtual std::any visit_decl_variable(const std::shared_ptr<decl::Variable> &d) override;

    /* Statements.
     *
     * Similar to top level declarations, statements can be replaced with single statements
     * by returning a shared_ptr<stmt::Statement>, or with multiple statements by returning
     * a vector<shared_ptr<stmt::Statement>>
     * Returned statements must be cast up to the parent type as a
     * std::shared_ptr<stmt::Statement>
     */
    virtual std::any visit_stmt_block(const std::shared_ptr<stmt::Block> &s) override;
    virtual std::any visit_stmt_if_else(const std::shared_ptr<stmt::IfElse> &s) override;
    virtual std::any visit_stmt_while(const std::shared_ptr<stmt::While> &s) override;
    virtual std::any visit_stmt_for(const std::shared_ptr<stmt::For> &s) override;
    virtual std::any visit_stmt_return(const std::shared_ptr<stmt::Return> &s) override;
    virtual std::any visit_stmt_variable_declaration(
        const std::shared_ptr<stmt::VariableDeclaration> &s) override;
    virtual std::any visit_stmt_expression(
        const std::shared_ptr<stmt::Expression> &s) override;

    /* Expressions
     *
     * Expressions and subexpressions can be removed by returning an empty std::any.
     * Returned expressions must be cast up to the parent type as a
     * std::shared_ptr<expr::Expression>
     */
    virtual std::any visit_expr_unary(const std::shared_ptr<expr::Unary> &e) override;
    virtual std::any visit_expr_binary(const std::shared_ptr<expr::Binary> &e) override;
    virtual std::any visit_expr_variable(const std::shared_ptr<expr::Variable> &e) override;
    virtual std::any visit_expr_constant(const std::shared_ptr<expr::Constant> &e) override;
    virtual std::any visit_expr_function_call(
        const std::shared_ptr<expr::FunctionCall> &e) override;
    virtual std::any visit_struct_array_access(
        const std::shared_ptr<expr::StructArrayAccess> &e) override;
    virtual std::any visit_expr_assignment(
        const std::shared_ptr<expr::Assignment> &e) override;

private:
    /* Utility for nodes that can accept zero, one, or multiple values returned by visiting the
     * child node passed. Results will be appended to results
     */
    template <typename ResultT>
    void collect_results(const std::any &result,
                         std::vector<std::shared_ptr<ResultT>> &results) const;

    // Get back the std::shared_ptr<ResultT> returned, or nullptr
    template <typename ResultT>
    std::shared_ptr<ResultT> result_or_nullptr(const std::any &res) const;
};

template <typename ResultT>
void ModifyingVisitor::collect_results(const std::any &result,
                                       std::vector<std::shared_ptr<ResultT>> &results) const
{
    if (!result.has_value()) {
        return;
    }
    if (result.type() == typeid(std::shared_ptr<ResultT>)) {
        auto decl = std::any_cast<std::shared_ptr<ResultT>>(result);
        results.push_back(decl);
    } else if (result.type() == typeid(std::vector<std::shared_ptr<ResultT>>)) {
        auto node_results = std::any_cast<std::vector<std::shared_ptr<ResultT>>>(result);
        for (auto &r : node_results) {
            results.push_back(r);
        }
    } else {
        const std::string type_name = result.type().name();
        const std::string valid_single_res = typeid(std::shared_ptr<ResultT>).name();
        const std::string valid_multiple_res =
            typeid(std::vector<std::shared_ptr<ResultT>>).name();

        std::cout << "ModifyingVisitor: visit returned invalid type '" << type_name
                  << "', expected empty std::any, " << valid_single_res << ", or "
                  << valid_multiple_res << "\n";
        throw std::runtime_error("ModifyingVisitor: visit returned invalid type '" +
                                 type_name + "'");
    }
}

template <typename ResultT>
std::shared_ptr<ResultT> ModifyingVisitor::result_or_nullptr(const std::any &res) const
{
    return res.has_value() ? std::any_cast<std::shared_ptr<ResultT>>(res) : nullptr;
}
}
}
