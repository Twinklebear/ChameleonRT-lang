#pragma once

#include "node.h"
#include "type.h"

namespace crtl {
namespace ast {

struct Symbol {
    std::string name;
    antlr4::Token *token = nullptr;
    std::shared_ptr<ty::Type> type;

    Symbol(const std::string &name, antlr4::Token *token);

    // Generated symbol's tokens are null, as there's no source input they correspond too
    // TODO: Maybe this could refer to the original source token that caused their generation?
    // might get hard to track with more complex generation of nodes or not make much sense.
    // making it null will help catch bugs
    bool is_generated() const;
};
}
}
