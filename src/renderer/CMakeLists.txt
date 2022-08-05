find_package(Threads REQUIRED)

find_package(SDL2 CONFIG REQUIRED)
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


# Include glm as an external project
if (NOT TARGET glm)
    include(${PROJECT_SOURCE_DIR}/cmake/glm.cmake)
endif()

add_executable(crtl_test_renderer main.cpp)

set_target_properties(crtl_test_renderer PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

target_link_libraries(crtl_test_renderer PUBLIC
    glm
    SDL2::SDL2
    SDL2::SDL2main
    crtl)

