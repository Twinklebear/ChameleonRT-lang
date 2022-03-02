#include "error_listener.h"
#include <iostream>

using namespace antlr4;

namespace crtl {

void ErrorReporter::report_error(const antlr4::Token *token, const std::string &msg)
{
    had_error = true;
    report(token, "Error", msg);
}

void ErrorReporter::report_warning(const antlr4::Token *token, const std::string &msg)
{
    report(token, "Warning", msg);
}

void ErrorReporter::report(const antlr4::Token *token,
                           const std::string &prefix,
                           const std::string &msg)
{
    std::cerr << prefix << " at " << token->getLine() << ":" << token->getCharPositionInLine()
              << " '" << token->getText() << "' > " << msg << "\n"
              << std::flush;
}

void ErrorListener::syntaxError(
    Recognizer *, Token *, size_t, size_t, const std::string &, std::exception_ptr)
{
    // It looks like there's a default error handler attached
    // that prints out errors (the console error listener)
    // So here just flag an error happened.
    had_syntax_error = true;
}

bool ErrorListener::had_error() const
{
    return had_syntax_error;
}

}
