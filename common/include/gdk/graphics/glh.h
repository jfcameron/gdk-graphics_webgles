// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GLH_H
#define GDK_GLH_H

#include <gdk/graphics/types.h>
#include <gdk/graphics/opengl.h>
#include <jfc/unique_handle.h>

#include <optional>
#include <string>
#include <string_view>

namespace gdk::graphics {
    struct color;
}

namespace glh {
    void ClearColor(const gdk::graphics::color &acolor);

    void Viewport(const gdk::graphics::intvector2_type &aPos, 
        const gdk::graphics::intvector2_type &aSize);
    
    void Scissor(const gdk::graphics::intvector2_type &aPos, 
        const gdk::graphics::intvector2_type &aSize);
    
    // Error detection & logging
    std::string GetShaderInfoLog(const GLuint aShaderStageHandle);
    std::string GetProgramInfoLog(const GLuint ashader_programHandle);
    std::optional<std::string> GetError();
}

#endif

