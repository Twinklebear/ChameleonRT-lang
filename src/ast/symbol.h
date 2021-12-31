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
};
}
}
