// © Joseph Cameron - MIT License

#include <gdk/graphics/webgl1es2_gl_state.h>

#include "test_include.h"

#include <jfc/catch.hpp>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_profile.h>
#include <gdk/graphics/webgl1es2_scene.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/webgl1es2_texture_camera.h>
#include <gdk/graphics/webgl1es2_volume_texture.h>

#include <array>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    constexpr int LENGTH = 16;

    [[nodiscard]] constexpr int channel_for(const int aCoordinate) { return aCoordinate * 17; }

    [[nodiscard]] std::vector<texture_data::channel_type> a_volume_of_its_own_coordinates() {
        std::vector<texture_data::channel_type> out(LENGTH * LENGTH * LENGTH * 3);

        for (int z = 0; z < LENGTH; ++z)
            for (int y = 0; y < LENGTH; ++y)
                for (int x = 0; x < LENGTH; ++x) {
                    const auto index = static_cast<std::size_t>((x + y * LENGTH + z * LENGTH * LENGTH) * 3);

                    out[index] = static_cast<texture_data::channel_type>(channel_for(x));
                    out[index + 1] = static_cast<texture_data::channel_type>(channel_for(y));
                    out[index + 2] = static_cast<texture_data::channel_type>(channel_for(z));
                }

        return out;
    }

    [[nodiscard]] texture_data::volume_view view_of(const std::vector<texture_data::channel_type> &aTexels) {
        return {.width = LENGTH, .height = LENGTH, .depth = LENGTH, .format = volume_texture::format::rgb,
            .data = aTexels.data()};
    }

    struct probe final {
        std::vector<texture_data::channel_type> texels = a_volume_of_its_own_coordinates();

        std::shared_ptr<webgl1es2_shader_program> pShader =
            std::make_shared<webgl1es2_shader_program>(R"V0G0N(
            attribute highp vec3 a_Position;

            void main() { gl_Position = vec4(a_Position.xy * 4.0, 0.0, 1.0); }
            )V0G0N", R"V0G0N(
            uniform sampler2D _Volume;
            uniform mediump float _Volume_Size;
            uniform mediump vec3 _Volume_Wrap;
            uniform mediump float _Volume_Smooth;
            uniform mediump vec3 _Position;

            void main() {
                gl_FragColor = gdk_volume(_Volume, _Volume_Size, _Volume_Wrap, _Volume_Smooth, _Position);
            }
            )V0G0N");

        std::shared_ptr<webgl1es2_material> pMaterial = std::make_shared<webgl1es2_material>(
            pShader, material::face_culling_mode::none, material::render_mode::opaque);

        std::shared_ptr<webgl1es2_entity> pEntity =
            std::make_shared<webgl1es2_entity>(webgl1es2_model::make_cube(), pMaterial);

        std::shared_ptr<webgl1es2_texture_camera> pCamera =
            std::make_shared<webgl1es2_texture_camera>(intvector2_type(4, 4));

        webgl1es2_scene scene{std::make_shared<gl_state>()};

        explicit probe(const volume_texture::wrap_mode aU = volume_texture::wrap_mode::clamped,
            const volume_texture::wrap_mode aV = volume_texture::wrap_mode::clamped,
            const volume_texture::wrap_mode aW = volume_texture::wrap_mode::clamped,
            const volume_texture::filter_mode aFilter = volume_texture::filter_mode::sharp) {
            pMaterial->set_volume_texture("_Volume", std::make_shared<webgl1es2_volume_texture>(view_of(texels),
                aU, aV, aW, aFilter, nullptr));

            pEntity->set_transform({0, 0, -3}, quaternion_type::identity);
            pCamera->set_perspective_projection(1.0f, 0.1f, 100.0f, 1.0f);
            pCamera->set_transform(vector3_type::zero, quaternion_type::identity);

            static_cast<void>(pCamera->color_texture());

            scene.add(pEntity);
            scene.add(pCamera);
        }

        ~probe() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        [[nodiscard]] std::array<int, 3> at(const float aX, const float aY, const float aZ) {
            pMaterial->set_vector3("_Position", {aX / LENGTH, aY / LENGTH, aZ / LENGTH});

            scene.draw({4, 4});

            std::array<std::uint8_t, 4> pixel{};
            glReadPixels(1, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel.data());

            return {pixel[0], pixel[1], pixel[2]};
        }
    };

    void require_near(const std::array<int, 3> &aGot, const std::array<float, 3> &aExpected) {
        for (std::size_t channel = 0; channel < 3; ++channel)
            REQUIRE(static_cast<float>(aGot[channel]) == Approx(aExpected[channel]).margin(1.0f));
    }
}

TEST_CASE("**a volume texture is read at a position by gdk_volume, given only what its material sets**",
    "[volume_texture]")
{
    initGL();

    probe subject;

    SECTION("a voxel in the middle") {
        REQUIRE(subject.at(3.5f, 7.5f, 12.5f) == std::array<int, 3>{51, 119, 204});
    }

    SECTION("the first and last voxels") {
        REQUIRE(subject.at(0.5f, 0.5f, 0.5f) == std::array<int, 3>{0, 0, 0});
        REQUIRE(subject.at(15.5f, 15.5f, 15.5f) == std::array<int, 3>{255, 255, 255});
    }

    SECTION("past every edge, clamped, it reads the edge") {
        REQUIRE(subject.at(20.5f, -3.5f, 16.5f) == std::array<int, 3>{255, 0, 255});
    }

    REQUIRE(!webgles_test::glGetError());
}

TEST_CASE("**a volume texture wraps each axis as it was made**", "[volume_texture]")
{
    initGL();

    probe subject(volume_texture::wrap_mode::repeat, volume_texture::wrap_mode::mirrored,
        volume_texture::wrap_mode::clamped);

    SECTION("repeat on x, mirrored on y, clamped on z, past the high edges") {
        REQUIRE(subject.at(19.5f, 18.5f, 20.5f)
            == std::array<int, 3>{channel_for(3), channel_for(13), channel_for(15)});
    }

    SECTION("and past the low edges") {
        REQUIRE(subject.at(-0.5f, -0.5f, -0.5f)
            == std::array<int, 3>{channel_for(15), channel_for(0), channel_for(0)});
    }

    REQUIRE(!webgles_test::glGetError());
}

TEST_CASE("**a volume texture wraps each axis by its own mode**", "[volume_texture]")
{
    initGL();

    probe subject(volume_texture::wrap_mode::clamped, volume_texture::wrap_mode::repeat,
        volume_texture::wrap_mode::mirrored);

    REQUIRE(subject.at(20.5f, 19.5f, 18.5f) == std::array<int, 3>{channel_for(15), channel_for(3), channel_for(13)});

    REQUIRE(!webgles_test::glGetError());
}

TEST_CASE("**a smooth volume texture blends the eight voxels around a position**", "[volume_texture]")
{
    initGL();

    probe subject(volume_texture::wrap_mode::clamped, volume_texture::wrap_mode::clamped,
        volume_texture::wrap_mode::clamped, volume_texture::filter_mode::smooth);

    SECTION("at a voxel's centre it is that voxel") {
        require_near(subject.at(3.5f, 7.5f, 12.5f), {51.0f, 119.0f, 204.0f});
    }

    SECTION("on the face between two it is their average, and at a corner of eight, theirs") {
        require_near(subject.at(4.0f, 7.5f, 12.5f), {17.0f * 3.5f, 119.0f, 204.0f});
        require_near(subject.at(4.0f, 8.0f, 13.0f), {17.0f * 3.5f, 17.0f * 7.5f, 17.0f * 12.5f});
    }

    REQUIRE(!webgles_test::glGetError());
}

TEST_CASE("**a box written into a volume texture is read back in place**", "[volume_texture]")
{
    initGL();

    probe subject;

    auto pVolume = std::make_shared<webgl1es2_volume_texture>(view_of(subject.texels), volume_texture::wrap_mode::clamped,
        volume_texture::wrap_mode::clamped, volume_texture::wrap_mode::clamped, volume_texture::filter_mode::sharp,
        nullptr);

    subject.pMaterial->set_volume_texture("_Volume", pVolume);

    std::vector<texture_data::channel_type> box(3 * 2 * 2 * 3);

    for (std::size_t i = 0; i < box.size(); ++i) box[i] = static_cast<texture_data::channel_type>(0x20 + i / 3);

    pVolume->update_data({.width = 3, .height = 2, .depth = 2, .format = volume_texture::format::rgb,
        .data = box.data()}, 5, 6, 7);

    REQUIRE(subject.at(5.5f, 6.5f, 7.5f) == std::array<int, 3>{0x20, 0x20, 0x20});
    REQUIRE(subject.at(7.5f, 6.5f, 7.5f) == std::array<int, 3>{0x22, 0x22, 0x22});
    REQUIRE(subject.at(5.5f, 7.5f, 7.5f) == std::array<int, 3>{0x23, 0x23, 0x23});
    REQUIRE(subject.at(6.5f, 6.5f, 8.5f) == std::array<int, 3>{0x27, 0x27, 0x27});
    REQUIRE(subject.at(7.5f, 7.5f, 8.5f) == std::array<int, 3>{0x2B, 0x2B, 0x2B});
    REQUIRE(subject.at(8.5f, 6.5f, 7.5f) == std::array<int, 3>{channel_for(8), channel_for(6), channel_for(7)});
    REQUIRE(subject.at(5.5f, 6.5f, 9.5f) == std::array<int, 3>{channel_for(5), channel_for(6), channel_for(9)});

    REQUIRE(!webgles_test::glGetError());
}

TEST_CASE("**a volume texture refuses what does not pack into a square, saying why**", "[volume_texture]")
{
    initGL();

    const std::vector<texture_data::channel_type> texels(16 * 16 * 16 * 4, 0);

    const auto make = [&texels](const std::size_t aWidth, const std::size_t aHeight, const std::size_t aDepth,
        std::shared_ptr<const profile> apProfile = nullptr) {
        return webgl1es2_volume_texture({.width = aWidth, .height = aHeight, .depth = aDepth,
            .format = volume_texture::format::rgba, .data = texels.data()}, volume_texture::wrap_mode::clamped,
            volume_texture::wrap_mode::clamped, volume_texture::wrap_mode::clamped, volume_texture::filter_mode::sharp,
            std::move(apProfile));
    };

    SECTION("not a cube") {
        REQUIRE_THROWS_WITH(make(16, 16, 4), Catch::Contains("must be a cube"));
    }

    SECTION("an edge that is not a perfect square") {
        REQUIRE_THROWS_WITH(make(8, 8, 8), Catch::Contains("8 is not"));
    }

    SECTION("no texels") {
        REQUIRE_THROWS_WITH(make(0, 0, 0), Catch::Contains("must be a cube with texels"));
    }

    SECTION("packed past the profile's largest texture: an edge of 16 packs 64 across") {
        auto small = es2_minimum();
        small.most.texture_size = 32;

        REQUIRE_THROWS_AS(make(16, 16, 16, std::make_shared<const profile>(small)), exception);
        REQUIRE_NOTHROW(make(4, 4, 4, std::make_shared<const profile>(small)));
    }

    REQUIRE(!webgles_test::glGetError());
}
