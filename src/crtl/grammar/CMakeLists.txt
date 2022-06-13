# Setup ANTLR
set(ANTLR4_WITH_STATIC_CRT OFF)
include(ExternalAntlr4Cpp)
find_package(ANTLR REQUIRED)

include(ANTLRGrammarUtil)

# Add the ChameleonRT Grammar
antlr_target(crtl_grammar
    PACKAGE crtg
    LEXER ChameleonRTLexer.g4
    PARSER ChameleonRTParser.g4
    VISITOR)

antlr_grammar_util(ChameleonRTGrammarUtil
    LEXER ChameleonRTLexer.g4
    PARSER ChameleonRTParser.g4)

target_include_directories(crtl_grammar PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/antlr4java_generated_src/
    ${ANTLR4_INCLUDE_DIRS})

target_compile_definitions(crtl_grammar PUBLIC
    -DANTLR4CPP_STATIC)

