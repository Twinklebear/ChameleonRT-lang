add_executable(test_renderer main.cpp)

set_target_properties(test_renderer PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

target_link_libraries(test_renderer PUBLIC
    util
    display
    crtl_dxr)

