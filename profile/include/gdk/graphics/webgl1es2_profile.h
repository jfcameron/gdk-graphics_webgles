// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_PROFILE_H
#define GDK_GFX_WEBGL1ES2_PROFILE_H

#include <gdk/graphics/profile.h>

#include <string_view>

/// \file
/// \brief the ES 2 targets' guaranteed minimums, and a GLSL ES 1.00 program checked against a profile by its
/// declarations. Needs no GL: a headless consumer links `gdk::graphics_webgles_profile` alone.
namespace gdk::graphics {
    //! what every OpenGL ES 2.0 device has: the spec's guaranteed minimums
    [[nodiscard]] profile es2_minimum();

    //! what every WebGL 1 browser has: WebGL 1 guarantees the ES 2.0 minimums, so the same limits
    [[nodiscard]] profile webgl1_minimum();

    /// \brief a GLSL ES 1.00 program checked against a profile, counted from its declarations: uniform vectors in
    /// each stage, attributes, varyings, and samplers in each stage
    ///
    /// What webgl1es2_context checks its shaders with. A null context given it counts shaders the same way:
    /// `null_context::make(webgl1_minimum(), check_es2_shader)`.
    /// \throws gdk::graphics::exception past any of the profile's limits
    void check_es2_shader(const profile &aProfile, std::string_view aVertexSource, std::string_view aFragmentSource);
}

#endif
