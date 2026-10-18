# © Joseph Cameron - All Rights Reserved

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/webgl1es2_profile_test.cpp"

    LIBRARIES
        gdkgraphics_webgles_profile
)
