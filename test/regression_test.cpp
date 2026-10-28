// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "test_include.h"

#include <gdk/graphics/exception.h>
#include <gdk/graphics/webgl1es2_context.h>
#include <gdk/graphics/webgl1es2_gl_state.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_profile.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/webgl1es2_texture.h>

#include <array>
#include <map>
#include <memory>
#include <string>

using namespace gdk::graphics;

namespace {
    constexpr const char *FRAGMENT = "varying vec2 v; void main(){ gl_FragColor = vec4(v, 0.0, 1.0); }";

    [[nodiscard]] std::array<unsigned char, 4> pixel_at(const int aX, const int aY) {
        std::array<unsigned char, 4> out{};

        glReadPixels(aX, aY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, out.data());

        return out;
    }

    [[nodiscard]] std::shared_ptr<screen_camera> clearing(webgl1es2_context &aContext, const color &aColor,
        const float aX, const float aWidth) {
        auto pCamera = aContext.make_camera();

        pCamera->set_clear_color(aColor);
        pCamera->set_viewport(aX, 0, aWidth, 1);

        return pCamera;
    }
}

TEST_CASE("a model bound to a program that took a dropped program's gl name binds afresh", "[webgles][model]") {
    initGL();

    const auto pContext = webgl1es2_context::make();

    const auto pModel = std::static_pointer_cast<webgl1es2_model>(pContext->make_cube_model());

    std::map<std::string, GLint> before;

    GLuint name = 0;

    {
        const auto pFirst = std::static_pointer_cast<const webgl1es2_shader_program>(pContext->make_shader(
            "attribute vec3 a_Position; attribute vec2 a_UV; varying vec2 v; uniform mat4 _MVP; "
            "void main(){ v = a_UV; gl_Position = _MVP * vec4(a_Position, 1.0); }", FRAGMENT));

        name = pFirst->handle();

        pModel->bind(*pFirst);
    }

    const auto pSecond = std::static_pointer_cast<const webgl1es2_shader_program>(pContext->make_shader(
        "attribute vec2 a_UV; attribute vec3 a_Position; attribute vec3 a_Normal; varying vec2 v; uniform mat4 _MVP; "
        "void main(){ v = a_UV + a_Normal.xy; gl_Position = _MVP * vec4(a_Position, 1.0); }", FRAGMENT));

    if (pSecond->handle() != name) WARN("gl gave the second program a name of its own; the reuse was not exercised");

    pModel->bind(*pSecond);

    for (const auto &[attribute, info] : pSecond->activeAttributes()) {
        INFO(attribute);

        GLint enabled = 0;

        glGetVertexAttribiv(static_cast<GLuint>(info.location), GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);

        REQUIRE(enabled == GL_TRUE);
    }
}

TEST_CASE("viewports that meet leave no column of the window undrawn", "[webgles][camera]") {
    initGL();

    const auto pContext = webgl1es2_context::make();

    const auto pBackground = pContext->make_scene();
    pBackground->add(clearing(*pContext, color::blue, 0, 1));
    pBackground->draw({399, 300});

    const auto pScene = pContext->make_scene();
    const auto pLeft = clearing(*pContext, color::red, 0, 0.5f);
    const auto pRight = clearing(*pContext, color::green, 0.5f, 0.5f);

    pScene->add(pLeft);
    pScene->add(pRight);
    pScene->draw({399, 300});

    REQUIRE(pixel_at(0, 10)[0] == 255);
    REQUIRE(pixel_at(398, 10)[1] == 255);

    for (const auto x : {199, 200}) {
        const auto shown = pixel_at(x, 10);

        REQUIRE(shown[2] == 0);
    }
}

TEST_CASE("a camera added twice is drawn once", "[webgles][scene]") {
    initGL();

    const auto pContext = webgl1es2_context::make();
    const auto pScene = pContext->make_scene();
    const auto pCamera = pContext->make_camera();

    pCamera->set_perspective_projection(1.5f, 0.01f, 100, 1);
    pCamera->set_transform({0, 0, 5}, quaternion_type::identity);

    const auto pEntity = pContext->make_entity(pContext->make_cube_model(),
        pContext->make_material(pContext->make_alpha_cutoff_shader()));

    pScene->add(pCamera);
    pScene->add(pCamera);
    pScene->add(pEntity);

    static_cast<void>(pContext->take_counts());

    pScene->draw({64, 64});

    REQUIRE(pContext->take_counts().draws == 1);
}

TEST_CASE("each material's textures take units from the first", "[webgles][material]") {
    initGL();

    std::string samplers, sum;

    for (int i = 0; i < 10; ++i) {
        samplers += "uniform sampler2D _T" + std::to_string(i) + "; ";
        sum += "texture2D(_T" + std::to_string(i) + ", vec2(0.5)) + ";
    }

    const auto pProgram = std::make_shared<webgl1es2_shader_program>(
        std::string("attribute vec3 a_Position; void main(){ gl_Position = vec4(a_Position, 1.0); }"),
        samplers + "void main(){ gl_FragColor = " + sum + "vec4(0.0); }");

    gl_state state;

    state.limit_texture_units(8);

    webgl1es2_material first(pProgram, material::face_culling_mode::none, material::render_mode::opaque);
    webgl1es2_material second(pProgram, material::face_culling_mode::none, material::render_mode::opaque);

    for (int i = 0; i < 5; ++i) {
        first.set_texture("_T" + std::to_string(i), webgl1es2_texture::make_checkerboard_of_death());
        second.set_texture("_T" + std::to_string(i + 5), webgl1es2_texture::make_checkerboard_of_death());
    }

    REQUIRE_NOTHROW(first.activate(state));
    REQUIRE_NOTHROW(second.activate(state));

    SECTION("though one material's own textures are still held to the limit") {
        webgl1es2_material greedy(pProgram, material::face_culling_mode::none, material::render_mode::opaque);

        for (int i = 0; i < 10; ++i)
            greedy.set_texture("_T" + std::to_string(i), webgl1es2_texture::make_checkerboard_of_death());

        REQUIRE_THROWS_AS(greedy.activate(state), exception);
    }
}

TEST_CASE("a texture camera is held to the profile, and gives its depth when asked", "[webgles][texture_camera]") {
    initGL();

    auto small = webgl1_minimum();

    small.most.texture_size = 64;

    const auto pHeld = webgl1es2_context::make(small);

    REQUIRE_NOTHROW(pHeld->make_texture_camera({60, 40}));
    REQUIRE_THROWS_AS(pHeld->make_texture_camera({128, 128}), exception);
    REQUIRE_THROWS_AS(pHeld->make_texture_camera({0, 16}), exception);

    const auto pContext = webgl1es2_context::make();
    const auto pTarget = pContext->make_texture_camera({60, 40});

    REQUIRE(pTarget->color_texture());

    const auto pDepth = pTarget->depth_texture();

    REQUIRE(pDepth);
    REQUIRE(pTarget->depth_texture() == pDepth);
    REQUIRE(!webgles_test::glGetError());
}
