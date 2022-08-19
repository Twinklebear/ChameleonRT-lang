include(GenerateExportHeader)

add_library(crtl SHARED
    crtl_buffer.cpp
    crtl_device.cpp
    crtl_event.cpp
    crtl_geometry.cpp
    crtl_parameter_block.cpp
    crtl_queue.cpp
    crtl_rtpipeline.cpp
    crtl_shader.cpp
    crtl_texture.cpp

    backend_plugin.cpp
    device.cpp
    util.cpp
    error.cpp
    type.cpp
)

set_target_properties(crtl PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

# TODO: need install include paths set up later
target_include_directories(crtl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external>)

target_link_libraries(crtl PUBLIC glm)

generate_export_header(crtl)

add_subdirectory(dxr)

#install(FILES
#    ${PROJECT_BINARY_DIR}/crtl_export.h DESTINATION ${INCLUDE_INSTALL_DIR}
#)
