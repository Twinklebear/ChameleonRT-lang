cmake_minimum_required(VERSION 3.5)
project(chameleonrtlang)

include(ExternalProject)

add_definitions(-DNOMINMAX -DGLM_ENABLE_EXPERIMENTAL -DSDL_MAIN_HANDLED
    -DWIN32_LEAN_AND_MEAN)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

option(CHAMELEONRT_LANG_COMPILER "Build the ChameleonRT-Language Compiler" ON)
if (CHAMELEONRT_LANG_COMPILER)
    add_subdirectory(compiler)
endif()

option(CHAMELEONRT_LANG_RUNTIME "Build the ChameleonRT-Language Runtime and Demo Renderer" OFF)
if (CHAMELEONRT_LANG_RUNTIME)
    find_package(Threads REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    # Include glm as an external project
    include(cmake/glm.cmake)

    add_subdirectory(imgui)
    add_subdirectory(util)

    add_subdirectory(runtime)
    add_subdirectory(renderer)
endif()

