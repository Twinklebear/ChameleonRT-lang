find_package(D3D12 REQUIRED)

add_dxil_embed_library(dxr_shaders
    small.hlsl
	COMPILE_OPTIONS -O3
	COMPILE_DEFINITIONS
        ${HLSL_COMPILE_DEFNS}
	INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_LIST_DIR}
        ${PROJECT_SOURCE_DIR})

add_library(crtl_dxr
    dxdisplay.cpp
    dx12_utils.cpp
    dxr_utils.cpp
    imgui_impl_dx12.cpp)

set_target_properties(crtl_dxr PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

target_include_directories(crtl_dxr PRIVATE
	$<BUILD_INTERFACE:${D3D12_INCLUDE_DIRS}>)

target_include_directories(crtl_dxr PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_link_libraries(crtl_dxr PUBLIC
	dxr_shaders
    util
    display
    ${D3D12_LIBRARIES})

