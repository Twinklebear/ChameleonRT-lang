add_subdirectory(grammar)

# TODO: Maybe shared in the future, especially if backends
# will be loaded at runtime
add_library(crtl STATIC
    compiler/ast_builder_visitor.cpp
    compiler/ast_expr_builder_visitor.cpp
    compiler/ast_struct_array_access_builder_visitor.cpp
    compiler/error_listener.cpp
    compiler/json_visitor.cpp
    compiler/resolver_visitor.cpp
    compiler/builtins.cpp
    compiler/rename_entry_point_param_visitor.cpp
    compiler/global_struct_param_expansion_visitor.cpp
    compiler/parameter_transforms.cpp

    compiler/ast/node.cpp
    compiler/ast/symbol.cpp
    compiler/ast/type.cpp
    compiler/ast/declaration.cpp
    compiler/ast/statement.cpp
    compiler/ast/expression.cpp
    compiler/ast/visitor.cpp
    compiler/ast/modifying_visitor.cpp

    compiler/hlsl/shader_register_binding.cpp
    compiler/hlsl/shader_register_allocator.cpp
    compiler/hlsl/translate_builtin_type.cpp
    compiler/hlsl/translate_builtin_function_call.cpp
    compiler/hlsl/output_visitor.cpp
    compiler/hlsl/parameter_metadata_output_visitor.cpp

    shader_library.cpp
)

# TODO: Later these includes will be private
target_include_directories(crtl PUBLIC
    # TODO: Eventually remove the compiler include path here
    ${CMAKE_CURRENT_LIST_DIR}/compiler/
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/external)

target_link_libraries(crtl PUBLIC chameleonrt_grammar)
