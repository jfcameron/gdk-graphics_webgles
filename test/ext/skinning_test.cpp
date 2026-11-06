// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/graphics/ext/webgl1es2_skinning.h>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/webgl1es2_profile.h>

#include <array>
#include <string>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;
using Catch::Matchers::Contains;

namespace {
    constexpr const char *MAIN = "uniform highp mat4 _MVP;\nuniform highp mat4 _Model;\n"
        "attribute highp vec3 a_Position;\n"
        "void main() { gl_Position = _MVP * _Model * skinning() * vec4(a_Position, 1.0); }\n";

    constexpr const char *FRAGMENT = "void main() { gl_FragColor = vec4(1.0); }\n";
}

TEST_CASE("**a skinned vertex shader has the bones the profile leaves beside its own uniforms, three vectors each**",
    "[skinning]") {
    CHECK(es2_most_bones(webgl1_minimum(), 8) == 40);
    CHECK(es2_most_bones(webgl1_minimum(), 0) == 42);
    CHECK(es2_most_bones(webgl1_minimum(), 125) == 1);
    CHECK(es2_most_bones(webgl1_minimum(), 126) == 0);
    CHECK(es2_most_bones(webgl1_minimum(), 200) == 0);
    CHECK(es2_most_bones(profile{"open", {}}, 8) > 1'000'000);
}

TEST_CASE("**the skinning is counted as it is sized, and a profile takes it up to its most bones**", "[skinning]") {
    const auto most = es2_most_bones(webgl1_minimum(), 8);

    REQUIRE_NOTHROW(check_es2_shader(webgl1_minimum(), es2_skinning(most) + MAIN, FRAGMENT));
    REQUIRE_THROWS_WITH(check_es2_shader(webgl1_minimum(), es2_skinning(most + 1) + MAIN, FRAGMENT),
        Contains("vertex uniform vectors"));

    const auto made = es2_skinning(16);

    CHECK_THAT(made, Contains("attribute highp vec2 a_Joints;") && Contains("attribute highp float a_Weight;")
        && Contains("uniform highp vec4 _Bones[48];") && Contains("highp mat4 skinning()"));

    REQUIRE_THROWS_AS(es2_skinning(0), exception);
}

TEST_CASE("**a palette goes in as each matrix's top three rows, in order**", "[skinning]") {
    auto moved = matrix4x4_type::identity;
    moved.set_translation({1, 2, 3});

    auto filled = matrix4x4_type::identity;
    for (std::size_t column = 0; column < 4; ++column)
        for (std::size_t row = 0; row < 3; ++row)
            filled.set(column, row, static_cast<float>(10 * row + column));

    const std::array palette{moved, filled};
    const auto rows = es2_bone_rows(palette);

    REQUIRE(rows.size() == 6);
    CHECK(rows[0] == vector4_type{1, 0, 0, 1});
    CHECK(rows[1] == vector4_type{0, 1, 0, 2});
    CHECK(rows[2] == vector4_type{0, 0, 1, 3});
    CHECK(rows[3] == vector4_type{0, 1, 2, 3});
    CHECK(rows[4] == vector4_type{10, 11, 12, 13});
    CHECK(rows[5] == vector4_type{20, 21, 22, 23});

    CHECK(es2_bone_rows({}).empty());
}
