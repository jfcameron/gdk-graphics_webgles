# © Joseph Cameron - MIT License

add_library(gdkgraphics_webgles_profile STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/webgl1es2_profile.cpp")

add_library(gdk::graphics_webgles_profile ALIAS gdkgraphics_webgles_profile)

set_target_properties(gdkgraphics_webgles_profile PROPERTIES EXPORT_NAME graphics_webgles_profile PREFIX "lib")

target_include_directories(gdkgraphics_webgles_profile
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_compile_features(gdkgraphics_webgles_profile PUBLIC cxx_std_20)

target_link_libraries(gdkgraphics_webgles_profile PUBLIC gdkgraphics)
