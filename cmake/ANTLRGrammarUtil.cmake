find_package(Java QUIET)
if (WIN32)
    find_file(ANTLR_GRAMMAR_UTIL_SCRIPT NAME ANTLRGrammarUtil.ps1.in
        PATHS
        ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_LIST_DIR})
else()
    find_file(ANTLR_GRAMMAR_UTIL_SCRIPT NAME ANTLRGrammarUtil.sh.in
        PATHS
        ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_LIST_DIR})
endif()

function(antlr_grammar_util)
    set(ANTLR_ONE_VALUE_ARGS LEXER PARSER)
    cmake_parse_arguments(PARSE_ARGV 1 ANTLR_UTIL
        ""
        "${ANTLR_ONE_VALUE_ARGS}"
        "")
    if (NOT ANTLR_UTIL_LEXER OR NOT ANTLR_UTIL_PARSER)
        message(FATAL_ERROR "Both the lexer and parser must be provided")
    endif()

    get_filename_component(ANTLR_INPUT_LEXER ${ANTLR_UTIL_LEXER} NAME_WE)
    get_filename_component(ANTLR_INPUT_PARSER ${ANTLR_UTIL_PARSER} NAME_WE)
    set(OUTPUT_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/antlr4java_generated_src/${ANTLR_INPUT_PARSER})

    set(LEXER_JAVA_SRC
        ${OUTPUT_DIR}/${ANTLR_INPUT_LEXER}.java)

    set(LEXER_OUTPUTS
        ${OUTPUT_DIR}/${ANTLR_INPUT_LEXER}.interp
        ${OUTPUT_DIR}/${ANTLR_INPUT_LEXER}.tokens
        ${LEXER_JAVA_SRC})

    add_custom_command(
        OUTPUT ${LEXER_OUTPUTS}
        COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
            ${ANTLR_UTIL_LEXER}
            -o ${OUTPUT_DIR}
        DEPENDS
            ${ANTLR_UTIL_LEXER}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building util for lexer ${ANTLR_INPUT_LEXER}")

    set(PARSER_JAVA_SRC
        ${OUTPUT_DIR}/${ANTLR_INPUT_PARSER}.java)

    set(PARSER_OUTPUTS
        ${OUTPUT_DIR}/${ANTLR_INPUT_PARSER}.interp
        ${OUTPUT_DIR}/${ANTLR_INPUT_PARSER}.tokens
        ${PARSER_JAVA_SRC})

    add_custom_command(
        OUTPUT ${PARSER_OUTPUTS}
        COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
            ${ANTLR_UTIL_PARSER}
            -o ${OUTPUT_DIR}
            -no-listener
            -no-visitor
        DEPENDS
            ${ANTLR_UTIL_PARSER}
            ${LEXER_OUTPUTS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building util for lexer ${ANTLR_INPUT_LEXER}")

    set(JAVA_CLASSES
        ${OUTPUT_DIR}/${ANTLR_INPUT_LEXER}.class
        ${OUTPUT_DIR}/${ANTLR_INPUT_PARSER}.class)

    add_custom_command(
        OUTPUT ${JAVA_CLASSES}
        COMMAND ${Java_JAVAC_EXECUTABLE}
            -classpath "${OUTPUT_DIR}"
            -classpath "${ANTLR_EXECUTABLE}"
            ${LEXER_JAVA_SRC}
            ${PARSER_JAVA_SRC}
        DEPENDS
            ${LEXER_JAVA_SRC} ${PARSER_JAVA_SRC}
        WORKING_DIRECTORY ${OUTPUT_DIR}
        COMMENT "Compiling ${ANTLR_INPUT} Java Files")

    string(REGEX REPLACE "Lexer" "" ANTLR_GRAMMAR_NAME ${ANTLR_INPUT_LEXER})
    if (${ANTLR_GRAMMAR_NAME} STREQUAL ${ANTLR_INPUT_LEXER})
        message(FATAL_ERROR "The lexer must be named <name>Lexer and the parser <name>Parser")
    endif()

    if (WIN32)
        configure_file(${ANTLR_GRAMMAR_UTIL_SCRIPT} ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.ps1 @ONLY)
        add_custom_target(${ARGV0} ALL DEPENDS ${JAVA_CLASSES} ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.ps1)
    else()
        configure_file(${ANTLR_GRAMMAR_UTIL_SCRIPT} ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.sh @ONLY)
        add_custom_target(${ARGV0} ALL DEPENDS ${JAVA_CLASSES} ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.sh)
    endif()
endfunction()

