find_package(Threads REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

# Include glm as an external project
include(${PROJECT_SOURCE_DIR}/cmake/glm.cmake)

add_subdirectory(imgui)
add_subdirectory(util)

add_executable(test_renderer main.cpp)

set_target_properties(test_renderer PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

target_link_libraries(test_renderer PUBLIC
    util
    display
    # TODO: Need to link just the final CRTL lib
    crtl)

