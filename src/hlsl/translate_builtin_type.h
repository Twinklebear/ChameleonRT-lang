#pragma once

#include <set>
#include <string>
#include "ast/type.h"

namespace crtl {
namespace hlsl {

std::string translate_modifiers(const std::set<ast::ty::Modifier> &modifiers);

/* Translate the passed built-in type to the corresponding HLSL type string. Types that can be
 * translated are
 * - Primitive
 * - Vector
 * - Matrix
 * - Buffer
 * - Texture
 * - AccelerationStructure
 * - Ray
 */
std::string translate_builtin_type(const std::shared_ptr<ast::ty::Type> &type);
}
}
