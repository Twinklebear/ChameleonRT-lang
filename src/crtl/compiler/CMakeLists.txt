add_library(crtl_compiler SHARED
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

# TODO: Later these includes will be private
target_include_directories(crtl_compiler PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/external)

target_link_libraries(crtl_compiler PRIVATE crtl_grammar)

