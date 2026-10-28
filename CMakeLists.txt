# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

if (TARGET gdkgraphics_webgles)
    return()
endif()

project(gdk-graphics_webgles
    VERSION 1.0
    DESCRIPTION "the gdk graphics interface, implemented over OpenGL ES 2 and WebGL 1"
    LANGUAGES C CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(GDKGRAPHICS_WEBGLES_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(GDKGRAPHICS_WEBGLES_BUILD_DEMO "Build the demos" OFF)
option(GDKGRAPHICS_WEBGLES_BUILD_DOCS "Build documentation" OFF)
option(GDKGRAPHICS_WEBGLES_INSTALL "Generate install rules, a vendored GLEW's included" ${PROJECT_IS_TOP_LEVEL})

if (GDKGRAPHICS_WEBGLES_BUILD_DOCS AND NOT GDKGRAPHICS_WEBGLES_BUILD_TESTS AND NOT GDKGRAPHICS_WEBGLES_BUILD_DEMO)
    add_subdirectory(docs)

    return()
endif()

set(GDKGRAPHICS_WEBGLES_USES_GLEW OFF)

if (NOT EMSCRIPTEN AND NOT ANDROID AND NOT APPLE)
    set(GDKGRAPHICS_WEBGLES_USES_GLEW ON)
endif()

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

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_subdirectory(profile)

add_library(gdkgraphics_webgles STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_entity.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_material.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_model.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_scene.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_screen_camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_shader_program.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_texture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_texture_camera.cpp")

add_library(gdk::graphics_webgles ALIAS gdkgraphics_webgles)

set_target_properties(gdkgraphics_webgles PROPERTIES EXPORT_NAME graphics_webgles PREFIX "lib")

jfc_generate_build_info(INCLUDE_PATH "gdk/graphics_webgles")

target_include_directories(gdkgraphics_webgles
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
        "${PROJECT_BINARY_DIR}/generated_include")

target_compile_features(gdkgraphics_webgles PUBLIC cxx_std_20)

target_link_libraries(gdkgraphics_webgles
    PUBLIC
        gdkgraphics
        gdkgraphics_webgles_profile
    PRIVATE
        $<BUILD_INTERFACE:gdkgraphics_implementation>
        $<BUILD_INTERFACE:jfc-smart_handles>)

set(_gdkgraphics_webgles_find_gl "")

if (EMSCRIPTEN)
    target_link_options(gdkgraphics_webgles PUBLIC "-lembind")
elseif (ANDROID)
    target_link_libraries(gdkgraphics_webgles PRIVATE GLESv2 EGL)
elseif (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    target_link_libraries(gdkgraphics_webgles PRIVATE "-framework OpenGLES")
else()
    set(OpenGL_GL_PREFERENCE "GLVND")

    find_package(OpenGL REQUIRED)

    target_link_libraries(gdkgraphics_webgles PRIVATE OpenGL::GL)

    set(_gdkgraphics_webgles_find_gl "set(OpenGL_GL_PREFERENCE GLVND)\nfind_dependency(OpenGL)\n")

    if (GDKGRAPHICS_WEBGLES_USES_GLEW)
        target_link_libraries(gdkgraphics_webgles PRIVATE GLEW)
    endif()

    if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        if (TARGET OpenGL::GLX)
            target_link_libraries(gdkgraphics_webgles PRIVATE OpenGL::GLX)

            set(_gdkgraphics_webgles_find_gl
                "set(OpenGL_GL_PREFERENCE GLVND)\nfind_dependency(OpenGL COMPONENTS GLX)\n")
        endif()

        find_package(OpenGL QUIET COMPONENTS EGL)

        if (TARGET OpenGL::EGL)
            target_link_libraries(gdkgraphics_webgles PRIVATE OpenGL::EGL)

            target_compile_definitions(gdkgraphics_webgles PUBLIC GDK_GRAPHICS_HAS_EGL=1)

            set(_gdkgraphics_webgles_find_gl
                "set(OpenGL_GL_PREFERENCE GLVND)\nfind_dependency(OpenGL COMPONENTS GLX EGL)\n")
        endif()
    endif()
endif()

add_library(gdkgraphics_webgles_private INTERFACE)

target_include_directories(gdkgraphics_webgles_private INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
    "${PROJECT_BINARY_DIR}/generated_include")

target_link_libraries(gdkgraphics_webgles_private INTERFACE
    gdkgraphics_webgles gdkgraphics_implementation jfc-smart_handles
    $<$<BOOL:${GDKGRAPHICS_WEBGLES_USES_GLEW}>:GLEW>
    $<$<TARGET_EXISTS:OpenGL::GL>:OpenGL::GL>)

if (GDKGRAPHICS_WEBGLES_INSTALL)
    set(_gdkgraphics_webgles_installed gdkgraphics_webgles gdkgraphics_webgles_profile)

    if (GDKGRAPHICS_WEBGLES_USES_GLEW AND GDKGRAPHICS_WEBGLES_VENDORED_GLEW)
        list(APPEND _gdkgraphics_webgles_installed GLEW)
    endif()

    install(TARGETS ${_gdkgraphics_webgles_installed}
        EXPORT gdk-graphics_webgles-targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

    install(DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        "${CMAKE_CURRENT_SOURCE_DIR}/profile/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(EXPORT gdk-graphics_webgles-targets
        FILE gdk-graphics_webgles-targets.cmake
        NAMESPACE gdk::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-graphics_webgles")

    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/gdk-graphics_webgles-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    file(WRITE "${PROJECT_BINARY_DIR}/gdk-graphics_webgles-config.cmake"
"include(CMakeFindDependencyMacro)
find_dependency(gdk-graphics 1)
${_gdkgraphics_webgles_find_gl}include(\"\${CMAKE_CURRENT_LIST_DIR}/gdk-graphics_webgles-targets.cmake\")
")

    install(FILES
        "${PROJECT_BINARY_DIR}/gdk-graphics_webgles-config.cmake"
        "${PROJECT_BINARY_DIR}/gdk-graphics_webgles-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-graphics_webgles")
endif()

if (GDKGRAPHICS_WEBGLES_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (GDKGRAPHICS_WEBGLES_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if (GDKGRAPHICS_WEBGLES_BUILD_DOCS)
    add_subdirectory(docs)
endif()
