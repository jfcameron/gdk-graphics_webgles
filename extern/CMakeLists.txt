# © Joseph Cameron - MIT License

if (NOT TARGET jfc-smart_handles)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/jfc-smart_handles")
endif()

if (NOT TARGET gdkgraphics)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-graphics")
endif()

if ((GDKGRAPHICS_WEBGLES_BUILD_TESTS OR GDKGRAPHICS_WEBGLES_BUILD_DEMO) AND NOT TARGET gdkwindowing_glfw
    AND NOT EMSCRIPTEN AND NOT ANDROID AND NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-windowing_glfw")
endif()

if (GDKGRAPHICS_WEBGLES_BUILD_DEMO AND NOT TARGET gdktiming)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-timing")
endif()

set(GDKGRAPHICS_WEBGLES_VENDORED_GLEW OFF PARENT_SCOPE)

if (GDKGRAPHICS_WEBGLES_USES_GLEW AND NOT TARGET GLEW)
    add_library(GLEW STATIC "${CMAKE_CURRENT_SOURCE_DIR}/glew-2.3.1/src/glew.c")

    target_include_directories(GLEW PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glew-2.3.1/include>")

    target_compile_definitions(GLEW PUBLIC GLEW_STATIC GLEW_NO_GLU)

    set(OpenGL_GL_PREFERENCE "GLVND")

    find_package(OpenGL REQUIRED)

    target_link_libraries(GLEW PUBLIC OpenGL::GL $<$<TARGET_EXISTS:OpenGL::GLX>:OpenGL::GLX>)

    set_target_properties(GLEW PROPERTIES PREFIX "lib" C_STANDARD 99)

    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/glew-2.3.1/src/glew.c" PROPERTIES
        COMPILE_OPTIONS "$<$<C_COMPILER_ID:MSVC>:/WX->;$<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-w>")

    set(GDKGRAPHICS_WEBGLES_VENDORED_GLEW ON PARENT_SCOPE)
endif()
