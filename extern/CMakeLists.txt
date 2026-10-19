# © Joseph Cameron - All Rights Reserved

set(_jfc_outer_build_tests "${JFC_BUILD_TESTS}")
set(_jfc_outer_build_docs "${JFC_BUILD_DOCS}")
set(_jfc_outer_build_demo "${JFC_BUILD_DEMO}")

set(JFC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JFC_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(JFC_BUILD_DEMO OFF CACHE BOOL "" FORCE)

if (NOT TARGET jfc-smart_handles)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/jfc-smart_handles")
endif()

if (NOT TARGET gdkgraphics)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-graphics")
endif()

if (_jfc_outer_build_tests OR _jfc_outer_build_demo)
    if (NOT TARGET gdkwindow)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-windowing")
    endif()
endif()

if (_jfc_outer_build_demo)
    if (NOT TARGET gdktime)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-time")
    endif()
endif()

if ((CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID) OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (NOT TARGET GLEW)
        add_library(GLEW STATIC "${CMAKE_CURRENT_SOURCE_DIR}/glew-2.1.0/src/glew.c")

        target_include_directories(GLEW PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glew-2.1.0/include>")

        set_target_properties(GLEW PROPERTIES PREFIX "lib")
    endif()
endif()

set(JFC_BUILD_TESTS "${_jfc_outer_build_tests}" CACHE BOOL "" FORCE)
set(JFC_BUILD_DOCS "${_jfc_outer_build_docs}" CACHE BOOL "" FORCE)
set(JFC_BUILD_DEMO "${_jfc_outer_build_demo}" CACHE BOOL "" FORCE)
