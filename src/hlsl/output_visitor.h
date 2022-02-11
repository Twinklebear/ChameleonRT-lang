#pragma once

#include "ast/visitor.h"
#include "resolver_visitor.h"
#include "shader_register_allocator.h"

namespace crtl {
namespace hlsl {

// TODO: Later can have a parent for all langage output visitors that will hold the generated
// source, metadata, etc.
class OutputVisitor : public ast::Visitor {
    std::shared_ptr<ResolverPassResult> resolver_result;

    ShaderRegisterAllocator register_allocator;

    // Map of global and entry point parameter names to their register binding information
    phmap::parallel_flat_hash_map<std::string, std::shared_ptr<ParameterRegisterBinding>>
        parameter_binding;

public:
    OutputVisitor(const std::shared_ptr<ResolverPassResult> &resolver_result);

    // NOTE: Most statements don't need any rewriting but we do still need to visit
    // everything to build the HLSL source code

    /* Visit the AST and translate it to HLSL. The std::any returned contains a std::string
     * with the translated source code
     */
    std::any visit_ast(const std::shared_ptr<ast::AST> &ast) override;

    // Declarations
    std::any visit_decl_function(const std::shared_ptr<ast::decl::Function> &d) override;
    std::any visit_decl_entry_point(const std::shared_ptr<ast::decl::EntryPoint> &d) override;
    std::any visit_decl_global_param(
        const std::shared_ptr<ast::decl::GlobalParam> &d) override;
    std::any visit_decl_struct(const std::shared_ptr<ast::decl::Struct> &d) override;
    std::any visit_decl_struct_member(
        const std::shared_ptr<ast::decl::StructMember> &d) override;
    std::any visit_decl_variable(const std::shared_ptr<ast::decl::Variable> &d) override;

    // Statements
    std::any visit_stmt_block(const std::shared_ptr<ast::stmt::Block> &s) override;
    std::any visit_stmt_if_else(const std::shared_ptr<ast::stmt::IfElse> &s) override;
    std::any visit_stmt_while(const std::shared_ptr<ast::stmt::While> &s) override;
    std::any visit_stmt_for(const std::shared_ptr<ast::stmt::For> &s) override;
    std::any visit_stmt_return(const std::shared_ptr<ast::stmt::Return> &s) override;
    std::any visit_stmt_variable_declaration(
        const std::shared_ptr<ast::stmt::VariableDeclaration> &s) override;
    std::any visit_stmt_expression(const std::shared_ptr<ast::stmt::Expression> &s) override;

    // Expressions
    std::any visit_expr_unary(const std::shared_ptr<ast::expr::Unary> &e) override;
    std::any visit_expr_binary(const std::shared_ptr<ast::expr::Binary> &e) override;
    std::any visit_expr_variable(const std::shared_ptr<ast::expr::Variable> &e) override;
    std::any visit_expr_constant(const std::shared_ptr<ast::expr::Constant> &e) override;
    std::any visit_expr_function_call(
        const std::shared_ptr<ast::expr::FunctionCall> &e) override;
    std::any visit_struct_array_access(
        const std::shared_ptr<ast::expr::StructArrayAccess> &e) override;
    std::any visit_expr_assignment(const std::shared_ptr<ast::expr::Assignment> &e) override;

private:
    /* Bind the passed global or entry point parameter to registers and return the HLSL source
     * for the binding. The ParameterRegisterBinding metadata will be stored in the
     * parameter_binding map
     */
    std::string bind_parameter(const ast::decl::Variable *param);

    /* Bind the passed built in parameter type (i.e. not a struct) to a shader register.
     */
    ShaderRegisterBinding bind_builtin_type_parameter(
        const std::shared_ptr<ast::ty::Type> &type);
};
}
}

