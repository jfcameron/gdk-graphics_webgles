# © Joseph Cameron - All Rights Reserved

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/camera_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/entity_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/material_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/model_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/scene_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/shader_program_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_include.h"
        "${CMAKE_CURRENT_LIST_DIR}/texture3d_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/texture_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/webgles_conformance.cpp"

    INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}"

    LIBRARIES
        gdkgraphics_webgles
        gdkwindow
        gdkgraphics_conformance
)
