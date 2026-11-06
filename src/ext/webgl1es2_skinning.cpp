// © Joseph Cameron - MIT License

#include <gdk/graphics/ext/webgl1es2_skinning.h>

#include <gdk/graphics/exception.h>

#include <limits>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    constexpr std::size_t BONE_VECTORS = 3;
}

std::size_t gdk::graphics::ext::es2_most_bones(const profile &aProfile, const std::size_t aOther) {
    const auto vectors = aProfile.most.vertex_uniform_vectors;

    return aOther >= vectors ? 0 : (vectors - aOther) / BONE_VECTORS;
}

std::string gdk::graphics::ext::es2_skinning(const std::size_t aBones) {
    if (!aBones) throw exception("es2_skinning: a skinned vertex shader needs a bone");

    if (aBones > std::numeric_limits<std::size_t>::max() / BONE_VECTORS)
        throw exception("es2_skinning: " + std::to_string(aBones) + " bones are more than a size can count");

    const std::string joints(ES2_JOINTS), weight(ES2_WEIGHT), bones(ES2_BONES);

    return "attribute highp vec2 " + joints + ";\n"
        "attribute highp float " + weight + ";\n"
        "uniform highp vec4 " + bones + "[" + std::to_string(aBones * BONE_VECTORS) + "];\n"
        "\n"
        "highp mat4 skinning() {\n"
        "    int first = int(" + joints + ".x + 0.5) * 3;\n"
        "    int second = int(" + joints + ".y + 0.5) * 3;\n"
        "    highp float rest = 1.0 - " + weight + ";\n"
        "    highp vec4 x = " + bones + "[first] * " + weight + " + " + bones + "[second] * rest;\n"
        "    highp vec4 y = " + bones + "[first + 1] * " + weight + " + " + bones + "[second + 1] * rest;\n"
        "    highp vec4 z = " + bones + "[first + 2] * " + weight + " + " + bones + "[second + 2] * rest;\n"
        "    return mat4(x.x, y.x, z.x, 0.0, x.y, y.y, z.y, 0.0, x.z, y.z, z.z, 0.0, x.w, y.w, z.w, 1.0);\n"
        "}\n";
}

std::vector<vector4_type> gdk::graphics::ext::es2_bone_rows(const std::span<const matrix4x4_type> aPalette) {
    std::vector<vector4_type> out;

    out.reserve(aPalette.size() * BONE_VECTORS);

    for (const auto &each : aPalette)
        for (std::size_t row = 0; row < BONE_VECTORS; ++row)
            out.emplace_back(each.get(0, row), each.get(1, row), each.get(2, row), each.get(3, row));

    return out;
}
