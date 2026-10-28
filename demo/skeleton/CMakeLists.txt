# © Joseph Cameron - MIT License

project(skeleton_demo)

add_executable(skeleton_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/content.cpp)

# the demos' update event
target_include_directories(skeleton_demo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/include")

target_link_libraries(skeleton_demo PRIVATE gdkgraphics_webgles gdkwindowing_glfw gdktiming)

target_compile_features(skeleton_demo PRIVATE cxx_std_20)
