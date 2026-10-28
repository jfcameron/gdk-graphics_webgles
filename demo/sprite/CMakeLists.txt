# © Joseph Cameron - MIT License

project(sprite_demo)

add_executable(sprite_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# the demos' update event
target_include_directories(sprite_demo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/include")

target_link_libraries(sprite_demo PRIVATE gdkgraphics_webgles gdkwindowing_glfw gdktiming)

target_compile_features(sprite_demo PRIVATE cxx_std_20)

