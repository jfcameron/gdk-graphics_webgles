// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <gdk/graphics/webgl1es2_profile.h>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/null_context.h>

#include <string>
#include <string_view>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    [[nodiscard]] std::string vertex_with(std::string_view aMore) {
        return std::string(aMore) + "\nattribute vec3 a_Position;\nvoid main() { gl_Position = vec4(a_Position, 1.0); }\n";
    }

    [[nodiscard]] std::string fragment_with(std::string_view aMore) {
        return "precision mediump float;\n" + std::string(aMore) + "\nvoid main() { gl_FragColor = vec4(1.0); }\n";
    }

    [[nodiscard]] std::string repeated(std::string_view aDeclaration, const int aTimes) {
        std::string out;

        for (int i = 0; i < aTimes; ++i) out += std::string(aDeclaration) + std::to_string(i) + ";\n";

        return out;
    }

    void check(std::string_view aVertexSource, std::string_view aFragmentSource) {
        check_es2_shader(webgl1_minimum(), aVertexSource, aFragmentSource);
    }
}

TEST_CASE("**the spec profiles are the specs' guaranteed minimums**", "[profile]") {
    const auto es2 = es2_minimum();

    REQUIRE(es2.name == "es2_minimum");
    REQUIRE(es2.most.texture_size == 64);
    REQUIRE_FALSE(es2.most.non_power_of_two_textures);
    REQUIRE(es2.most.vertex_uniform_vectors == 128);
    REQUIRE(es2.most.fragment_uniform_vectors == 16);
    REQUIRE(es2.most.texture_units == 8);
    REQUIRE(es2.most.vertex_texture_units == 0);
    REQUIRE(es2.most.vertex_attributes == 8);
    REQUIRE(es2.most.varying_vectors == 8);

    REQUIRE(webgl1_minimum().name == "webgl1_minimum");
    REQUIRE(webgl1_minimum().most == es2.most);
}

TEST_CASE("**a shader is counted from its declarations, a stage at a time**", "[profile][shader]") {
    REQUIRE_NOTHROW(check(vertex_with(""), fragment_with("")));

    SECTION("**fragment uniform vectors**, sixteen at most") {
        REQUIRE_NOTHROW(check(vertex_with(""), fragment_with(repeated("uniform vec4 u_", 16))));
        REQUIRE_THROWS_WITH(check(vertex_with(""), fragment_with(repeated("uniform vec4 u_", 17))),
            "webgl1_minimum: a fragment shader's uniform vectors 17, past webgl1_minimum's fragment uniform vectors "
            "of 16");
    }

    SECTION("**a matrix is a vector a column, and an array its size times its type**") {
        REQUIRE_NOTHROW(check(vertex_with("uniform mat4 u_Bones[30];\nuniform mat3 u_Normal;\n"
            "uniform highp vec4 u_A, u_B;"), fragment_with("")));
        REQUIRE_THROWS_WITH(check(vertex_with("uniform mat4 u_Bones[31];\nuniform mat3 u_Normal;\n"
            "uniform highp vec4 u_A, u_B;"), fragment_with("")), Catch::Contains("vertex shader's uniform vectors 129"));
    }

    SECTION("**an array sized by a #define or a const int is counted at that size**") {
        REQUIRE_NOTHROW(check(vertex_with("#define BONES 32\nuniform mat4 u_Bones[BONES];"), fragment_with("")));
        REQUIRE_THROWS_WITH(check(vertex_with("#define BONES 33\nuniform mat4 u_Bones[BONES];"),
            fragment_with("")), Catch::Contains("vertex shader's uniform vectors 132"));
        REQUIRE_THROWS_WITH(check(vertex_with("const int LIGHTS = 17;"),
            fragment_with("const int LIGHTS = 17;\nuniform vec4 u_Lights[LIGHTS];")),
            Catch::Contains("fragment shader's uniform vectors 17"));
    }

    SECTION("**a precision chosen by a #define is a precision**") {
        const std::string chosen = "#ifdef GL_FRAGMENT_PRECISION_HIGH\n#define HIGHP_IF_ANY highp\n#else\n"
            "#define HIGHP_IF_ANY mediump\n#endif\n";

        REQUIRE_NOTHROW(check(vertex_with(chosen + "uniform HIGHP_IF_ANY vec4 u_Across;"), fragment_with("")));
        REQUIRE_THROWS_WITH(check(vertex_with(""), fragment_with(chosen + "uniform HIGHP_IF_ANY vec4 u_Across[17];")),
            Catch::Contains("fragment shader's uniform vectors 17"));
    }

    SECTION("**an array whose size it cannot read is refused, rather than guessed**") {
        REQUIRE_THROWS_WITH(check(vertex_with("uniform vec4 u_Lights[COUNT];"), fragment_with("")),
            Catch::Contains("u_Lights[COUNT]"));
    }

    SECTION("**what is commented out is not declared**") {
        REQUIRE_NOTHROW(check(vertex_with(""),
            fragment_with("// " + repeated("uniform vec4 x_", 1) + "/*\n" + repeated("uniform vec4 y_", 20) + "*/")));
    }

    SECTION("**samplers against texture units**, and none in a vertex shader") {
        REQUIRE_NOTHROW(check(vertex_with(""), fragment_with(repeated("uniform sampler2D t_", 8))));
        REQUIRE_THROWS_WITH(check(vertex_with(""), fragment_with(repeated("uniform sampler2D t_", 9))),
            Catch::Contains("samplers 9, past webgl1_minimum's texture units of 8"));
        REQUIRE_THROWS_WITH(check(vertex_with("uniform sampler2D u_Height;"), fragment_with("")),
            Catch::Contains("vertex texture units of 0"));
    }

    SECTION("**attributes and varyings**, eight of each") {
        REQUIRE_THROWS_WITH(check(vertex_with(repeated("attribute vec4 a_", 8)), fragment_with("")),
            Catch::Contains("attribute vectors 9"));
        REQUIRE_THROWS_WITH(check(vertex_with(repeated("varying vec2 v_", 9)),
            fragment_with(repeated("varying vec2 v_", 9))), Catch::Contains("varying vectors 9"));
    }
}

TEST_CASE("**a null context given the check counts shaders the same way**", "[profile][shader][null_context]") {
    const auto pContext = null_context::make(webgl1_minimum(), check_es2_shader);

    REQUIRE(pContext->make_shader(vertex_with(""), fragment_with(repeated("uniform vec4 u_", 16))));
    REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with(""), fragment_with(repeated("uniform vec4 u_", 17))),
        Catch::Contains("webgl1_minimum: a fragment shader's uniform vectors 17"));
}
