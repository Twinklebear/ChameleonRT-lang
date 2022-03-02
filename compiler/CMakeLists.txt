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
    chameleonrtc.cpp
    ast_builder_visitor.cpp
    ast_expr_builder_visitor.cpp
    ast_struct_array_access_builder_visitor.cpp
    error_listener.cpp
    json_visitor.cpp
    resolver_visitor.cpp
    builtins.cpp
    rename_entry_point_param_visitor.cpp
    global_struct_param_expansion_visitor.cpp
    parameter_transforms.cpp

    ast/node.cpp
    ast/symbol.cpp
    ast/type.cpp
    ast/declaration.cpp
    ast/statement.cpp
    ast/expression.cpp
    ast/visitor.cpp
    ast/modifying_visitor.cpp

    hlsl/shader_register_binding.cpp
    hlsl/shader_register_allocator.cpp
    hlsl/translate_builtin_type.cpp
    hlsl/translate_builtin_function_call.cpp
    hlsl/output_visitor.cpp
    hlsl/parameter_metadata_output_visitor.cpp
)

target_include_directories(chameleonrtc PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/external)

target_link_libraries(chameleonrtc chameleonrt_grammar)

set_target_properties(chameleonrtc PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON)

if (WIN32)
    target_compile_definitions(chameleonrtc PRIVATE
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS=1)
endif()
