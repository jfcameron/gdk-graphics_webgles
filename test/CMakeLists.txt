# © Joseph Cameron - MIT License

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/profile_test.cpp"

    LIBRARIES
        gdkgraphics_webgles_profile)

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)

if (TARGET gdkwindowing_glfw)
    jfc_add_tests(
        C++_STANDARD 20

        TEST_SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/camera_test.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/context_test.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/entity_test.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/material_test.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/model_test.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/regression_test.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/scene_test.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/shader_program_test.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/texture3d_test.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/texture_test.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/webgles_conformance.cpp"

        INCLUDE_DIRECTORIES
            "${CMAKE_CURRENT_LIST_DIR}"

        LIBRARIES
            gdkgraphics_webgles_private
            gdkgraphics_conformance
            gdkwindowing_glfw)

    set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)
endif()

if (NOT CMAKE_CROSSCOMPILING AND GDKGRAPHICS_WEBGLES_INSTALL)
    add_test(NAME gdk-graphics_webgles_install_and_consume
        COMMAND "${CMAKE_COMMAND}"
            "-DBUILD_DIR=${CMAKE_BINARY_DIR}"
            "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/install_and_consume"
            "-DCONFIG=$<CONFIG>"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DPLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DTOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCXX_FLAGS=${CMAKE_CXX_FLAGS}"
            "-DEXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
            -P "${CMAKE_CURRENT_LIST_DIR}/install/install_and_consume.cmake")
endif()
