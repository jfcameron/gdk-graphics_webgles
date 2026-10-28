// © Joseph Cameron - MIT License

#include <gdk/graphics/color.h>
#include <gdk/graphics/glh.h>

#include <cstdio>
#include <vector>

namespace {
    [[nodiscard]] std::string trimmed(std::vector<GLchar> aLog) {
        std::string out(aLog.begin(), aLog.end());

        if (const auto end = out.find('\0'); end != std::string::npos) out.resize(end);

        return out;
    }
}

namespace glh {
    void ClearColor(const gdk::graphics::color &aColor) {
        glClearColor(aColor.r, aColor.g, aColor.b, aColor.a);
    }

    void Viewport(const gdk::graphics::intvector2_type &aPos, const gdk::graphics::intvector2_type &aSize) {
        glViewport(aPos.x, aPos.y, aSize.x, aSize.y);
    }

    void Scissor(const gdk::graphics::intvector2_type &aPos, const gdk::graphics::intvector2_type &aSize) {
        glScissor(aPos.x, aPos.y, aSize.x, aSize.y);
    }

    std::string GetShaderInfoLog(const GLuint aShaderStageHandle) {
        GLint length(0);
        glGetShaderiv(aShaderStageHandle, GL_INFO_LOG_LENGTH, &length);

        if (length <= 1) return {};

        std::vector<GLchar> log(static_cast<std::size_t>(length));
        glGetShaderInfoLog(aShaderStageHandle, length, nullptr, log.data());

        return trimmed(std::move(log));
    }

    std::string GetProgramInfoLog(const GLuint aProgramHandle) {
        GLint length(0);
        glGetProgramiv(aProgramHandle, GL_INFO_LOG_LENGTH, &length);

        if (length <= 1) return {};

        std::vector<GLchar> log(static_cast<std::size_t>(length));
        glGetProgramInfoLog(aProgramHandle, length, nullptr, log.data());

        return trimmed(std::move(log));
    }

    std::optional<std::string> GetError() {
        switch (const auto error = glGetError()) {
            case GL_NO_ERROR: return {};
            case GL_INVALID_ENUM: return {"GL_INVALID_ENUM"};
            case GL_INVALID_VALUE: return {"GL_INVALID_VALUE"};
            case GL_INVALID_OPERATION: return {"GL_INVALID_OPERATION"};
            case GL_INVALID_FRAMEBUFFER_OPERATION: return {"GL_INVALID_FRAMEBUFFER_OPERATION"};
            case GL_OUT_OF_MEMORY: return {"GL_OUT_OF_MEMORY"};

            default: {
                char name[32];
                std::snprintf(name, sizeof(name), "gl error 0x%04x", static_cast<unsigned>(error));

                return {name};
            }
        }
    }
}
