#pragma once

#include <parallel_hashmap/phmap.h>
#include "ast/declaration.h"
#include "global_struct_param_expansion_visitor.h"

namespace crtl {
using namespace ast;

/* Struct that stores the various parameter transform info created in the different passes so
 * that the final output step can save the right metadata to map from the original source code
 * to the final output names or locations of the variables
 */
struct ParameterTransforms {
    /* Global struct parameters that were expanded to individual global parameters
     * in the global struct param expansion pass
     */
    phmap::parallel_flat_hash_map<std::shared_ptr<ast::decl::GlobalParam>,
                                  std::shared_ptr<ExpandedGlobalParam>>
        expanded_global_params;

    /* Entry point parameters that were renamed to avoid name collisions in the
     * rename entry point param pass
     */
    phmap::parallel_flat_hash_map<std::shared_ptr<ast::decl::Variable>, std::string>
        renamed_vars;
};

}

