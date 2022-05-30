find_package(Java QUIET COMPONENTS Runtime)

if (NOT ANTLR_EXECUTABLE)
    find_program(ANTLR_EXECUTABLE
        NAMES antlr.jar antlr4.jar antlr-4.jar antlr-4.10.1-complete.jar)
endif()

if (ANTLR_EXECUTABLE AND Java_JAVA_EXECUTABLE)
    execute_process(
        COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
        OUTPUT_VARIABLE ANTLR_COMMAND_OUTPUT
        ERROR_VARIABLE ANTLR_COMMAND_ERROR
        RESULT_VARIABLE ANTLR_COMMAND_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(ANTLR_COMMAND_RESULT EQUAL 0)
        string(REGEX MATCH "Version [0-9]+(\\.[0-9]+)*" ANTLR_VERSION ${ANTLR_COMMAND_OUTPUT})
        string(REPLACE "Version " "" ANTLR_VERSION ${ANTLR_VERSION})
    else()
        message(
            SEND_ERROR
            "Command '${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}' "
            "failed with the output '${ANTLR_COMMAND_ERROR}'")
    endif()

    function(ANTLR_TARGET)
        set(ANTLR_OPTIONS LISTENER VISITOR)
        # TODO: Can also add a LEXER_PARSER option to still support single file lexer/parser
        set(ANTLR_ONE_VALUE_ARGS PACKAGE OUTPUT_DIRECTORY LEXER PARSER)
        set(ANTLR_MULTI_VALUE_ARGS COMPILE_FLAGS DEPENDS)
        cmake_parse_arguments(PARSE_ARGV 1 ANTLR_TARGET
            "${ANTLR_OPTIONS}"
            "${ANTLR_ONE_VALUE_ARGS}"
            "${ANTLR_MULTI_VALUE_ARGS}")

        set(ANTLR_TARGET_NAME ${ARGV0})

        if (ANTLR_TARGET_OUTPUT_DIRECTORY)
            set(ANTLR_OUTPUT_DIR ${ANTLR_TARGET_OUTPUT_DIRECTORY})
        else()
            set(ANTLR_OUTPUT_DIR
                ${CMAKE_CURRENT_BINARY_DIR}/antlr4cpp_generated_src/${ANTLR_INPUT})
        endif()

        if (ANTLR_TARGET_PACKAGE)
            list(APPEND ANTLR_TARGET_COMPILE_FLAGS -package ${ANTLR_TARGET_PACKAGE})
        endif()

        if (ANTLR_TARGET_LEXER)
            get_filename_component(ANTLR_LEXER_INPUT ${ANTLR_TARGET_LEXER} NAME_WE)
            set(ANTLR_LEXER_CXX_OUTPUTS
                ${ANTLR_OUTPUT_DIR}/${ANTLR_LEXER_INPUT}.h
                ${ANTLR_OUTPUT_DIR}/${ANTLR_LEXER_INPUT}.cpp)
            set(ANTLR_LEXER_OTHER_OUTPUTS
                ${ANTLR_OUTPUT_DIR}/${ANTLR_LEXER_INPUT}.interp
                ${ANTLR_OUTPUT_DIR}/${ANTLR_LEXER_INPUT}.tokens)

            add_custom_command(
                OUTPUT
                ${ANTLR_LEXER_CXX_OUTPUTS}
                ${ANTLR_LEXER_OTHER_OUTPUTS}
                COMMAND
                ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
                ${ANTLR_TARGET_LEXER}
                -o ${ANTLR_OUTPUT_DIR}
                -Dlanguage=Cpp
                ${ANTLR_TARGET_COMPILE_FLAGS}
                DEPENDS
                ${ANTLR_TARGET_LEXER}
                ${ANTLR_TARGET_DEPENDS}
                WORKING_DIRECTORY
                ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT
                "Building lexer ${ANTLR_LEXER_INPUT} with ANTLR ${ANTLR_VERSION}")
        endif()

        if (ANTLR_TARGET_PARSER)
            get_filename_component(ANTLR_PARSER_INPUT ${ANTLR_TARGET_PARSER} NAME_WE)
            set(ANTLR_PARSER_CXX_OUTPUTS
                ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}.h
                ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}.cpp)
            set(ANTLR_PARSER_OTHER_OUTPUTS
                ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}.interp
                ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}.tokens)

            if (ANTLR_TARGET_LISTENER)
                list(APPEND ANTLR_PARSER_CXX_OUTPUTS
                    ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}BaseListener.h
                    ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}BaseListener.cpp
                    ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}Listener.h
                    ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}Listener.cpp)

                list(APPEND ANTLR_TARGET_COMPILE_FLAGS -listener)
            else()
                list(APPEND ANTLR_TARGET_COMPILE_FLAGS -no-listener)
            endif()

            if (ANTLR_TARGET_VISITOR)
                list(APPEND ANTLR_PARSER_CXX_OUTPUTS
                    ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}BaseVisitor.h
                    ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}BaseVisitor.cpp
                    ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}Visitor.h
                    ${ANTLR_OUTPUT_DIR}/${ANTLR_PARSER_INPUT}Visitor.cpp)

                list(APPEND ANTLR_TARGET_COMPILE_FLAGS -visitor)
            else()
                list(APPEND ANTLR_TARGET_COMPILE_FLAGS -no-visitor)
            endif()

            add_custom_command(
                OUTPUT
                ${ANTLR_PARSER_CXX_OUTPUTS}
                ${ANTLR_PARSER_OTHER_OUTPUTS}
                COMMAND
                ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
                ${ANTLR_TARGET_PARSER}
                -o ${ANTLR_OUTPUT_DIR}
                -Dlanguage=Cpp
                ${ANTLR_TARGET_COMPILE_FLAGS}
                DEPENDS
                ${ANTLR_LEXER_OTHER_OUTPUTS}
                ${ANTLR_TARGET_PARSER}
                ${ANTLR_TARGET_DEPENDS}
                WORKING_DIRECTORY
                ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT
                "Building parser ${ANTLR_PARSER_INPUT} with ANTLR ${ANTLR_VERSION}")
        endif()

        add_library(${ANTLR_TARGET_NAME} ${ANTLR_LEXER_CXX_OUTPUTS} ${ANTLR_PARSER_CXX_OUTPUTS})
        target_link_libraries(${ANTLR_TARGET_NAME} PUBLIC
            antlr4_static)
        target_include_directories(${ANTLR_TARGET_NAME} PUBLIC
            ${ANTLR4_INCLUDE_DIRS}
            ${ANTLR_OUTPUT_DIR})
        set_target_properties(${ANTLR_TARGET_NAME} PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON)

    endfunction(ANTLR_TARGET)
endif(ANTLR_EXECUTABLE AND Java_JAVA_EXECUTABLE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    ANTLR
    REQUIRED_VARS ANTLR_EXECUTABLE Java_JAVA_EXECUTABLE
    VERSION_VAR ANTLR_VERSION)
