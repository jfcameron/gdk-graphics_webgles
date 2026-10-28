// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_GLH_H
#define GDK_GRAPHICS_GLH_H

#include <gdk/graphics/opengl.h>
#include <gdk/graphics/types.h>

#include <optional>
#include <string>

namespace gdk::graphics {
    struct color;
}

namespace glh {
    void ClearColor(const gdk::graphics::color &aColor);

    void Viewport(const gdk::graphics::intvector2_type &aPos, const gdk::graphics::intvector2_type &aSize);

    void Scissor(const gdk::graphics::intvector2_type &aPos, const gdk::graphics::intvector2_type &aSize);

    [[nodiscard]] std::string GetShaderInfoLog(const GLuint aShaderStageHandle);

    [[nodiscard]] std::string GetProgramInfoLog(const GLuint aProgramHandle);

    //! the oldest error gl has not yet reported, by name, or none
    [[nodiscard]] std::optional<std::string> GetError();
}

#endif
