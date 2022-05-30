add_subdirectory(display)

add_library(util
    util.cpp)

set_target_properties(util PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON)

target_include_directories(util PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external>)

target_link_libraries(util PUBLIC imgui glm)

if (NOT TARGET SDL2::SDL2)
    # Assume SDL2 is in the default library path and create
    # imported targets for it, we re-find the library since
    # SDL2_LIBRARIES might just be -lSDL2
    find_library(SDL2_SHARED_LIBRARY SDL2 REQUIRED)

    add_library(SDL2::SDL2 SHARED IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
        IMPORTED_LOCATION ${SDL2_SHARED_LIBRARY})
    target_include_directories(SDL2::SDL2 INTERFACE
        ${SDL2_INCLUDE_DIRS})

    # Same for SDL2main
    find_library(SDL2MAIN_SHARED_LIBRARY SDL2main REQUIRED)
    add_library(SDL2::SDL2main SHARED IMPORTED)
    set_target_properties(SDL2::SDL2main PROPERTIES
        IMPORTED_LOCATION ${SDL2MAIN_SHARED_LIBRARY})
endif()

target_link_libraries(util PUBLIC
    SDL2::SDL2
    SDL2::SDL2main)
install(IMPORTED_RUNTIME_ARTIFACTS SDL2::SDL2
    DESTINATION bin)

