cmake_minimum_required(VERSION 3.5)
project(chameleonrtlang)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

# Setup ANTLR
add_definitions(-DANTLR4CPP_STATIC)
set(ANTLR4_WITH_STATIC_CRT OFF)
include(ExternalAntlr4Cpp)
include_directories(${ANTLR4_INCLUDE_DIRS})
find_package(ANTLR REQUIRED)

include(ANTLRGrammarUtil)

# Add the ChameleonRT Grammar
antlr_target(chameleonrt_grammar
    PACKAGE crtg
    LEXER ChameleonRTLexer.g4
    PARSER ChameleonRTParser.g4
    VISITOR)

antlr_grammar_util(ChameleonRTGrammarUtil
    LEXER ChameleonRTLexer.g4
    PARSER ChameleonRTParser.g4)

add_executable(chameleonrtc
    src/chameleonrtc.cpp
    src/ast_builder_visitor.cpp
    src/ast_expr_builder_visitor.cpp
    src/ast_struct_array_access_builder_visitor.cpp
    src/error_listener.cpp
    src/json_visitor.cpp
    src/resolver_visitor.cpp

    src/ast/node.cpp
    src/ast/symbol.cpp
    src/ast/type.cpp
    src/ast/declaration.cpp
    src/ast/statement.cpp
    src/ast/expression.cpp
    src/ast/visitor.cpp
)

target_include_directories(chameleonrtc PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
    external)

target_link_libraries(chameleonrtc chameleonrt_grammar)

set_target_properties(chameleonrtc PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON)

if (WIN32)
    target_compile_definitions(chameleonrtc PRIVATE
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS=1)
endif()

