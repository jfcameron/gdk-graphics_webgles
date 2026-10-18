// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/color.h>
#include <gdk/graphics/glh.h>
#include <gdk/graphics/exception.h>

#include <vector>

namespace glh {
    void ClearColor(const gdk::graphics::color &acolor) {
        glClearColor(acolor.r, acolor.g, acolor.b, acolor.a);
    }

    void Viewport(const gdk::graphics::intvector2_type& aPos, const gdk::graphics::intvector2_type& aSize) {
        glViewport(aPos.x, aPos.y, aSize.x, aSize.y);
    }

    void Scissor(const gdk::graphics::intvector2_type& aPos, const gdk::graphics::intvector2_type& aSize) {
        glScissor(aPos.x, aPos.y, aSize.x, aSize.y);
    }

    std::string GetShaderInfoLog(const GLuint aShaderStageHandle) {
        GLint bufflen(0);
        glGetShaderiv(aShaderStageHandle, GL_INFO_LOG_LENGTH, &bufflen);
        
        if (bufflen > 1) {
            std::vector<GLchar> infoLog(bufflen);
            glGetShaderInfoLog(aShaderStageHandle, bufflen, 0, &infoLog[0]);
            
            return std::string(infoLog.begin(),infoLog.end());
        }
        
		return {};
    }

    std::string GetProgramInfoLog(const GLuint ashader_programHandle) {
        GLint maxLength(0);
        glGetProgramiv(ashader_programHandle, GL_INFO_LOG_LENGTH, &maxLength);
        
		if (maxLength) {
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(ashader_programHandle, maxLength, &maxLength, &infoLog[0]);

			return std::string(infoLog.begin(), infoLog.end());
		}

		return {};
    }
}

std::optional<std::string> glh::GetError() {
    switch (glGetError()) {
        case(GL_NO_ERROR): return {};
        case(GL_INVALID_ENUM): return {"GL_INVALID_ENUM"};
        case(GL_INVALID_VALUE): return {"GL_INVALID_VALUE"};
        case(GL_INVALID_OPERATION): return {"GL_INVALID_OPERATION"};
        case(GL_INVALID_FRAMEBUFFER_OPERATION): return {"GL_INVALID_FRAMEBUFFER_OPERATION"};
        case(GL_OUT_OF_MEMORY): return {"GL_OUT_OF_MEMORY"};

        default: break;
    }
    throw gdk::graphics::exception("unhandled gl error type");
}

