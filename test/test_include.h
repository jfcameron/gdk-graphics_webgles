// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_WEBGLES_TEST_INCLUDE_H
#define GDK_GRAPHICS_WEBGLES_TEST_INCLUDE_H

#include <gdk/graphics/opengl.h>
#include <gdk/windowing/glfw_context.h>

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

//! a window whose GL context is current, for the whole test run
inline void initGL() {
    static const auto pWindow = [] {
        static const auto pWindowing = gdk::windowing::glfw_context::make();

        auto pMade = pWindowing->make_window("gdk-graphics_webgles test", {400, 300});

#if GDK_GRAPHICS_GLEW
        glewExperimental = GL_TRUE;

        if (const GLenum err = glewInit(); err != GLEW_OK && err != GLEW_ERROR_NO_GLX_DISPLAY)
            throw std::runtime_error(std::string("initGL: glewInit failed: ")
                .append(reinterpret_cast<const char *>(glewGetErrorString(err))));
#endif

        return pMade;
    }();

    static_cast<void>(pWindow);
}

[[nodiscard]] inline std::optional<GLint> texture_level_width(const GLuint aHandle, const GLint aLevel)
{
#ifdef JFC_TARGET_PLATFORM_Emscripten
    static_cast<void>(aHandle);
    static_cast<void>(aLevel);

    return std::nullopt;
#else
    glBindTexture(GL_TEXTURE_2D, aHandle);

    GLint width = -1;

    glGetTexLevelParameteriv(GL_TEXTURE_2D, aLevel, GL_TEXTURE_WIDTH, &width);

    return width;
#endif
}

namespace webgles_test
{
    inline bool glGetError(const bool aExpected = false)
    {
        std::string errorcodebuffer = "";

        switch (::glGetError())
        {
            case(GL_NO_ERROR):
                errorcodebuffer = "GL_NO_ERROR";
                return false;

            case(GL_INVALID_ENUM):
                errorcodebuffer = "GL_INVALID_ENUM";
                break;

            case(GL_INVALID_VALUE):
                errorcodebuffer = "GL_INVALID_VALUE";
                break;

            case(GL_INVALID_OPERATION):
                errorcodebuffer = "GL_INVALID_OPERATION";
                break;

            //case(GL_INVALID_FRAMEBUFFER_OPERATION):
            //    errorcodebuffer = "GL_INVALID_FRAMEBUFFER_OPERATION";
                //break;

            case(GL_OUT_OF_MEMORY):
                errorcodebuffer = "GL_OUT_OF_MEMORY";
                break;

            default:
                errorcodebuffer = "gdk_UNHANDLED_GL_ERROR_CODE";
                break;

        }
        
        if (errorcodebuffer.size() && !aExpected) std::cerr << errorcodebuffer << std::endl;

        return true;
    }
}

#endif
