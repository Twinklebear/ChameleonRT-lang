add_executable(chameleonrtc
    chameleonrtc.cpp)

target_link_libraries(chameleonrtc PUBLIC
    crtl)

if (WIN32)
    target_compile_definitions(chameleonrtc PRIVATE
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS=1)
endif()

