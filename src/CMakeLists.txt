# TODO: Should move these defs to be on specific targets
add_definitions(-DNOMINMAX -DGLM_ENABLE_EXPERIMENTAL -DSDL_MAIN_HANDLED
    -DWIN32_LEAN_AND_MEAN)

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

add_subdirectory(crtl)

option(CHAMELEONRT_LANG_COMPILER "Build the ChameleonRT-Language Offline Compiler" ON)
if (CHAMELEONRT_LANG_COMPILER)
    add_subdirectory(chameleonrtc)
endif()

option(CHAMELEONRT_LANG_RENDERER "Build the ChameleonRT-Language Demo Renderer" OFF)
if (CHAMELEONRT_LANG_RENDERER)
    add_subdirectory(renderer)
endif()

