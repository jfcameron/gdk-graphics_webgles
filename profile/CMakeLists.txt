# © Joseph Cameron - All Rights Reserved

project(gdkgraphics_webgles_profile
    VERSION 0.9
    DESCRIPTION "game development kit graphics: the OpenGL ES 2 and WebGL 1 profiles and shader check, without GL")

add_library(gdkgraphics_webgles_profile STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_profile.cpp)

add_library(gdk::graphics_webgles_profile ALIAS gdkgraphics_webgles_profile)

set_target_properties(gdkgraphics_webgles_profile PROPERTIES EXPORT_NAME graphics_webgles_profile)

set_target_properties(gdkgraphics_webgles_profile PROPERTIES PREFIX "lib")

target_include_directories(gdkgraphics_webgles_profile
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>")

target_compile_features(gdkgraphics_webgles_profile PUBLIC cxx_std_20)

target_link_libraries(gdkgraphics_webgles_profile PUBLIC
    gdkgraphics)

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()
