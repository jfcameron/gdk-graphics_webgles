# © Joseph Cameron - All Rights Reserved

project(voxel_demo)

add_executable(voxel_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(voxel_demo PRIVATE gdkgraphics_webgles gdkwindow gdktime)

target_compile_features(voxel_demo PRIVATE cxx_std_20)

