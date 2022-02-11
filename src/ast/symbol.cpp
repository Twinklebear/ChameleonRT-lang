#include "symbol.h"

namespace crtl {
namespace ast {

Symbol::Symbol(const std::string &name, antlr4::Token *token) : name(name), token(token) {}

bool Symbol::is_generated() const
{
    return !token;
}
}
}
