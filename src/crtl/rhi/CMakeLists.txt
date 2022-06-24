include(GenerateExportHeader)

add_library(crtl_rhi SHARED
    crtl_device.cpp
    crtl_shader_library.cpp
    crtl_buffer.cpp
    backend_plugin.cpp
)

target_include_directories(crtl_rhi PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include/
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/external)

generate_export_header(crtl_rhi)

#add_subdirectory(dxr)

#install(FILES
#    ${PROJECT_BINARY_DIR}/crtl_rhi_export.h DESTINATION ${INCLUDE_INSTALL_DIR}
#)
