# © Joseph Cameron - All Rights Reserved

project(simple_text_demo)

# **the font gdk-graphics' data target ships**, wherever gdk-graphics is checked out: its own extern/, or a host's
get_target_property(_gdk_graphics_data_dir gdkgraphics_data SOURCE_DIR)

jfc_embed_file("EMBEDDED_FONT"
    "${_gdk_graphics_data_dir}/assets/fonts/LiberationSans-Regular.ttf"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/embedded_font.h"
)

add_executable(simple_text_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(simple_text_demo 
    PRIVATE
        # the demos' update event
        "${CMAKE_CURRENT_SOURCE_DIR}/../common/include"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_link_libraries(simple_text_demo PRIVATE gdkgraphics_webgles gdkwindow gdktime)

target_compile_features(simple_text_demo PRIVATE cxx_std_20)

