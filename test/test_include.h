// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_WEBGLES_TEST_INCLUDE_H
#define GDK_GRAPHICS_WEBGLES_TEST_INCLUDE_H

#include <gdk/graphics_webgles/build_info.h>

#include <stdexcept>

#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <iostream>
#include <optional>

inline void initGL()
{
    static bool bFirst = true;

    if (bFirst)
    {
        if (!glfwInit()) throw std::runtime_error(std::string("InitGL").append("/glfwInit failed"));

        glfwWindowHint(GLFW_RESIZABLE, true);

        if (GLFWwindow *pWindow = glfwCreateWindow(400, 300, "gdk window", nullptr, nullptr))
        {
            glfwMakeContextCurrent(pWindow);
            glfwSwapInterval(-1); 
            glfwSwapBuffers(pWindow);

#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
            if (const GLenum err = glewInit(); err != GLEW_OK)
                throw std::runtime_error(std::string("InitGL/glewInit failed: ")
                    .append(reinterpret_cast<const char *>(glewGetErrorString(err))));
#endif
        }
        else throw std::runtime_error(std::string("InitGL").append("/glfw window init failed"));

        bFirst = false;
    }
}

/// \brief **how wide a 2D texture's mip level is**, as GL says -- or nothing where it cannot be asked: WebGL has no
/// glGetTexLevelParameteriv, so a browser's run claims nothing of mip levels rather than an answer made up for it
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

//! **these tests' own helpers**, not a library's
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
