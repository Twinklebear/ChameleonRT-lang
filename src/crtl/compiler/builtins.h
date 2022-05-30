#pragma once

#include <memory>
#include <vector>
#include "ast/declaration.h"

namespace crtl {
std::vector<std::shared_ptr<ast::decl::Declaration>> get_builtin_decls();
}
