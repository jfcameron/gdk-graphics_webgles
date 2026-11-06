// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_WEBGL1ES2_SKINNING_H
#define GDK_GRAPHICS_EXT_WEBGL1ES2_SKINNING_H

#include <gdk/graphics/data_types.h>
#include <gdk/graphics/profile.h>

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

/// \file
/// \brief skinning in a GLSL ES 1.00 vertex shader: each bone's skinning matrix as three vec4 rows of a uniform array,
/// each vertex's two bones and its first bone's weight as attributes. Needs no GL: a headless consumer links
/// `gdk::graphics_webgles_skinning` alone.
///
/// A vertex shader starts with es2_skinning() and moves its vertices by skinning():
/// \code
///     gl_Position = _MVP * skinning() * vec4(a_Position, 1.0);
/// \endcode
/// Its model gives each vertex ES2_JOINTS and ES2_WEIGHT, and its material the palette each frame:
/// `set_vector4_array(ES2_BONES, es2_bone_rows(palette))`.
namespace gdk::graphics::ext {
    //! the uniform array holding the bones' rows
    inline constexpr std::string_view ES2_BONES = "_Bones";
    //! the vec2 attribute holding a vertex's two bones, as whole numbers
    inline constexpr std::string_view ES2_JOINTS = "a_Joints";
    //! the float attribute holding how much a vertex's first bone moves it; its second moves it the rest
    inline constexpr std::string_view ES2_WEIGHT = "a_Weight";

    /// \brief the most bones a vertex shader can skin with in aProfile, beside aOther uniform vectors of its own
    [[nodiscard]] std::size_t es2_most_bones(const profile &aProfile, std::size_t aOther);

    /// \brief GLSL ES 1.00 for the start of a vertex shader skinning with aBones bones: the two attributes, ES2_BONES
    /// sized three vec4s a bone, and `highp mat4 skinning()`, the vertex's two bones blended by its weight
    /// \throws gdk::graphics::exception for no bones, or more than a size can count
    [[nodiscard]] std::string es2_skinning(std::size_t aBones);

    /// \brief a skinning palette as the rows ES2_BONES takes: each matrix's top three rows, in order. The bottom row
    /// is taken to be 0, 0, 0, 1, as an affine transform's is
    [[nodiscard]] std::vector<vector4_type> es2_bone_rows(std::span<const matrix4x4_type> aPalette);
}

#endif
