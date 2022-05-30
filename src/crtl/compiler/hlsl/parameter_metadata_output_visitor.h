#pragma once

#include <memory>
#include "ast/visitor.h"
#include "parameter_transforms.h"
#include "resolver_visitor.h"
#include "shader_register_allocator.h"

namespace crtl {
namespace hlsl {

class ParameterMetadataOutputVisitor : public ast::Visitor {
    std::shared_ptr<ResolverPassResult> resolver_result;

    std::shared_ptr<ParameterTransforms> param_transforms;

    // Map of global and entry point parameter names to their register binding information
    phmap::parallel_flat_hash_map<std::shared_ptr<ast::decl::Variable>,
                                  std::shared_ptr<ParameterRegisterBinding>>
        parameter_bindings;

public:
    ParameterMetadataOutputVisitor(
        const std::shared_ptr<ResolverPassResult> &resolver_result,
        const std::shared_ptr<ParameterTransforms> &param_transforms,
        const phmap::parallel_flat_hash_map<std::shared_ptr<ast::decl::Variable>,
                                            std::shared_ptr<ParameterRegisterBinding>>
            &param_bindings);

    /* Visit the AST and build the parameter binding metadata JSON info for use at runtime.
     * Returns the nlohmann::json containing the parameter binding information for use by the
     * DXR runtime
     */
    std::any visit_ast(const std::shared_ptr<ast::AST> &ast) override;

    /* We just need to visit entry point and global param declarations to output their
     * parameters to JSON
     */
    std::any visit_decl_entry_point(const std::shared_ptr<ast::decl::EntryPoint> &d) override;
    std::any visit_decl_global_param(
        const std::shared_ptr<ast::decl::GlobalParam> &d) override;
};
}
}
