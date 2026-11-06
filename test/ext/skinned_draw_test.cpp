// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "test_include.h"

#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_gl_state.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_scene.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/ext/webgl1es2_skinning.h>
#include <gdk/graphics/webgl1es2_texture_camera.h>

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    constexpr int SIDE = 40;

    //! a square a fifth of the target wide about aX, aY in clip space, each corner on aJoints weighted aWeight
    [[nodiscard]] model_data square(const float aX, const float aY, const std::array<float, 2> aJoints,
        const float aWeight) {
        std::vector<float> positions, joints, weights;

        for (const auto [x, y] : std::array<std::array<float, 2>, 6>{{{-1, -1}, {1, -1}, {1, 1}, {-1, -1}, {1, 1},
            {-1, 1}}}) {
            positions.insert(positions.end(), {aX + x * 0.1f, aY + y * 0.1f, 0});
            joints.insert(joints.end(), aJoints.begin(), aJoints.end());
            weights.push_back(aWeight);
        }

        return model_data({{"a_Position", {positions, 3}}, {std::string(ES2_JOINTS), {joints, 2}},
            {std::string(ES2_WEIGHT), {weights, 1}}});
    }

    [[nodiscard]] matrix4x4_type moved(const float aX) {
        auto out = matrix4x4_type::identity;
        out.set_translation({aX, 0, 0});
        return out;
    }

    //! a quarter turn about z: x goes to y
    [[nodiscard]] matrix4x4_type turned() {
        auto out = matrix4x4_type::identity;
        out.set(0, 0, 0);
        out.set(0, 1, 1);
        out.set(1, 0, -1);
        out.set(1, 1, 0);
        return out;
    }

    //! which of the target's pixels a skinned model covers, drawn white on black
    struct target final {
        std::shared_ptr<webgl1es2_shader_program> pShader = std::make_shared<webgl1es2_shader_program>(
            es2_skinning(2) + "attribute highp vec3 a_Position;\n"
                "void main() { gl_Position = skinning() * vec4(a_Position, 1.0); }\n",
            "void main() { gl_FragColor = vec4(1.0); }\n");

        std::shared_ptr<webgl1es2_texture_camera> pCamera =
            std::make_shared<webgl1es2_texture_camera>(intvector2_type(SIDE, SIDE));

        std::shared_ptr<gl_state> pState = std::make_shared<gl_state>();

        target() {
            pCamera->set_perspective_projection(1.0f, 0.1f, 100.0f, 1.0f);
            pCamera->set_transform(vector3_type::zero, quaternion_type::identity);
            pCamera->set_clear_color(color(0, 0, 0, 1));

            static_cast<void>(pCamera->color_texture());
        }

        ~target() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

        void draw(const model_data &aModel, const std::array<matrix4x4_type, 2> &aPalette) {
            auto pMaterial = std::make_shared<webgl1es2_material>(pShader, material::face_culling_mode::none,
                material::render_mode::opaque);

            pMaterial->set_vector4_array(ES2_BONES, es2_bone_rows(aPalette));

            auto pEntity = std::make_shared<webgl1es2_entity>(
                std::make_shared<webgl1es2_model>(model::usage_hint::upload_once, aModel), pMaterial);

            pEntity->set_transform({0, 0, -3}, quaternion_type::identity);

            webgl1es2_scene scene{pState};

            scene.add(pEntity);
            scene.add(pCamera);
            scene.draw({SIDE, SIDE});
        }

        //! whether the pixel at clip space aX, aY was drawn
        [[nodiscard]] bool drawn(const float aX, const float aY) const {
            std::array<std::uint8_t, 4> pixel{};

            glReadPixels(static_cast<int>((aX + 1) / 2 * SIDE), static_cast<int>((aY + 1) / 2 * SIDE), 1, 1, GL_RGBA,
                GL_UNSIGNED_BYTE, pixel.data());

            return pixel[0] > 128;
        }
    };
}

TEST_CASE("**a skinned vertex moves by its two bones blended by its weight**", "[skinning][webgles]") {
    initGL();

    target screen;

    screen.draw(square(0, 0, {0, 1}, 0.75f), {moved(0.4f), moved(-0.4f)});

    CHECK(screen.drawn(0.2f, 0));
    CHECK_FALSE(screen.drawn(0, 0));
    CHECK_FALSE(screen.drawn(0.4f, 0));
    CHECK_FALSE(screen.drawn(-0.4f, 0));
    REQUIRE(!webgles_test::glGetError());
}

TEST_CASE("**a bone's rows turn a vertex as its matrix does**", "[skinning][webgles]") {
    initGL();

    target screen;

    screen.draw(square(0.5f, 0, {1, 0}, 1), {moved(0.4f), turned()});

    CHECK(screen.drawn(0, 0.5f));
    CHECK_FALSE(screen.drawn(0.5f, 0));
    CHECK_FALSE(screen.drawn(0, -0.5f));
    CHECK_FALSE(screen.drawn(0.9f, 0));
    REQUIRE(!webgles_test::glGetError());
}
