add_library(crtl_rhi SHARED
    shader_library.cpp
    shader_entry_point.cpp
)

target_link_libraries(crtl_rhi PRIVATE crtl_compiler)

#add_subdirectory(dxr)

