#include "parameter_transforms.h"

namespace crtl {
ParameterTransforms::ParameterTransforms(
    const phmap::parallel_flat_hash_map<std::shared_ptr<ast::decl::GlobalParam>,
                                        std::shared_ptr<ExpandedGlobalParam>>
        &in_expanded_global_params,
    const phmap::parallel_flat_hash_map<std::shared_ptr<ast::decl::Variable>, std::string>
        &in_renamed_vars)
    : expanded_global_params(in_expanded_global_params), renamed_vars(in_renamed_vars)
{
}

}
