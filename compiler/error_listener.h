#pragma once

#include "antlr4-runtime.h"

namespace crtl {

class ErrorReporter {
public:
    bool had_error = false;

    virtual ~ErrorReporter() = default;

    void report_error(const antlr4::Token *token, const std::string &msg);

    void report_warning(const antlr4::Token *token, const std::string &msg);

private:
    void report(const antlr4::Token *token, const std::string &prefix, const std::string &msg);
};

class ErrorListener : public antlr4::BaseErrorListener {
    bool had_syntax_error = false;

public:
    void syntaxError(antlr4::Recognizer *recognizer,
                     antlr4::Token *offendingSymbol,
                     size_t line,
                     size_t charPositionInLine,
                     const std::string &msg,
                     std::exception_ptr e) override;

    bool had_error() const;
};

}
