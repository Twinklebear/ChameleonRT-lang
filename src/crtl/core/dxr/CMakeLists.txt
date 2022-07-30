if (WIN32)
    option(CRTL_ENABLE_DXR "Enable the DXR backend for ChameleonRT-Lang" ON)
else()
    option(CRTL_ENABLE_DXR "Enable the DXR backend for ChameleonRT-Lang" OFF)
endif()

if (NOT CRTL_ENABLE_DXR)
    return()
endif()

find_package(D3D12 REQUIRED)

add_library(crtl_dxr SHARED
    crtl_dxr.cpp
    dxr_device.cpp
    dxr_utils.cpp
    dxr_resource.cpp
    dxr_buffer.cpp
    dxr_texture.cpp
    dxr_wrappers.cpp

    # TODO: cross platform windowing interaction is a different 
    # question, should allow getting native handles to objects in the API
    # The windowing stuff can be in a separate CRTL utility that uses
    # this native API handling
    #dxdisplay.cpp
)

set_target_properties(crtl_dxr PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

target_include_directories(crtl_dxr PRIVATE
	${D3D12_INCLUDE_DIRS}
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(crtl_dxr PRIVATE
    crtl
    ${D3D12_LIBRARIES})

generate_export_header(crtl_dxr)

