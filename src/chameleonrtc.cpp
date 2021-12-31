#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "ChameleonRTLexer.h"
#include "ChameleonRTParser.h"
#include "antlr4-runtime.h"
#include "ast_builder_visitor.h"
#include "error_listener.h"
#include "json_visitor.h"

using namespace crtg;

const std::string DIVIDER = "----------\n";

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: chameleonrtc [file]\n";
        return 1;
    }

    antlr4::ANTLRFileStream input;
    input.loadFromFile(argv[1]);

    crtl::ErrorListener error_listener;

    ChameleonRTLexer lexer(&input);
    lexer.addErrorListener(&error_listener);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();

    if (error_listener.had_error()) {
        std::cout << "Lexer error occured, exiting\n";
        return 1;
    }

    for (const auto &t : tokens.getTokens()) {
        std::cerr << t->toString() << "\n";
    }
    std::cout << DIVIDER;

    ChameleonRTParser parser(&tokens);
    parser.addErrorListener(&error_listener);
    antlr4::tree::ParseTree *tree = parser.file();

    if (error_listener.had_error()) {
        std::cout << "Parser error occured, exiting\n";
        return 1;
    }

    std::cerr << tree->toStringTree(&parser) << "\n" << DIVIDER;

    crtl::ASTBuilderVisitor ast_builder;
    ast_builder.visit(tree);
    if (ast_builder.had_error) {
        std::cout << "AST builder error occured, exiting\n";
        return 1;
    }

    std::cout << "AST built\n" << DIVIDER;

    auto ast = ast_builder.ast;

    crtl::JSONVisitor json_visitor;
    json_visitor.visit_ast(ast.get());

    std::cout << "AST JSON:\n" << json_visitor.ast_json.dump(4) << "\n";

    return 0;
}
