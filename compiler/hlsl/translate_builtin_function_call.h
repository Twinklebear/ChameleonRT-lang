#pragma once

#include "ast/declaration.h"
#include "ast/expression.h"

namespace crtl {
namespace hlsl {
// Translate the call expression calling the builtin function callee
std::string translate_builtin_function_call(ast::expr::FunctionCall *call,
                                            ast::decl::Function *callee);
}
}
