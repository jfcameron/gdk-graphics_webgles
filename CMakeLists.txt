# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

project(gdkgraphics_webgles
    VERSION 0.9
    DESCRIPTION "game development kit graphics, implemented over OpenGL ES 2 and WebGL 1")

option(JFC_BUILD_DEMO "Build the demos" ON)
option(JFC_BUILD_DOCS "Build documentation" OFF)
option(JFC_BUILD_TESTS "Build unit tests" OFF)

add_subdirectory(extern)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
    endif()

    include(CheckCXXCompilerFlag)

    foreach (_strict_flag "-Werror=c++20-extensions" "-Werror=c++23-extensions")
        string(MAKE_C_IDENTIFIER "HAS${_strict_flag}" _strict_flag_symbol)

        check_cxx_compiler_flag("${_strict_flag}" ${_strict_flag_symbol})

        if (${_strict_flag_symbol})
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_strict_flag}>)
        endif()
    endforeach()
endif()

add_library(gdkgraphics_webgles STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/glh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/src/webgl1es2_camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/src/webgl1es2_context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/src/webgl1es2_entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/src/webgl1es2_material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/src/webgl1es2_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/src/webgl1es2_scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/src/webgl1es2_screen_camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/src/webgl1es2_shader_program.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/src/webgl1es2_texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/src/webgl1es2_texture_camera.cpp)

add_library(gdk::graphics_webgles ALIAS gdkgraphics_webgles)

set_target_properties(gdkgraphics_webgles PROPERTIES EXPORT_NAME graphics_webgles)

set_target_properties(gdkgraphics_webgles PROPERTIES PREFIX "lib")

# **its own build info**, named for the repo whatever directory it is checked out into
jfc_generate_build_info(INCLUDE_PATH "gdk/graphics_webgles")

target_include_directories(gdkgraphics_webgles
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/webgl1es2/include>"
        "$<INSTALL_INTERFACE:include>"

        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated_include>"
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/common/include
    )

target_compile_features(gdkgraphics_webgles PUBLIC cxx_std_20)

target_link_libraries(gdkgraphics_webgles PUBLIC
    gdkgraphics)

if (EMSCRIPTEN)
    target_link_options(gdkgraphics_webgles PUBLIC "-lembind")
elseif (ANDROID)
    target_link_libraries(gdkgraphics_webgles PUBLIC GLESv2 EGL)
else()
    set(OpenGL_GL_PREFERENCE "GLVND")

    find_package(OpenGL REQUIRED)

    target_link_libraries(gdkgraphics_webgles PUBLIC OpenGL::GL)

    if (TARGET GLEW)
        target_link_libraries(gdkgraphics_webgles PUBLIC GLEW)
    endif()

    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        if (TARGET OpenGL::GLX)
            target_link_libraries(gdkgraphics_webgles PUBLIC OpenGL::GLX)
        endif()

        find_package(OpenGL QUIET COMPONENTS EGL)

        if (TARGET OpenGL::EGL)
            target_link_libraries(gdkgraphics_webgles PUBLIC OpenGL::EGL)

            target_compile_definitions(gdkgraphics_webgles PUBLIC GDK_GRAPHICS_HAS_EGL=1)
        endif()
    endif()
endif()

if (JFC_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()
