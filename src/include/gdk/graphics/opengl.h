// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_OPENGL_H
#define GDK_GRAPHICS_OPENGL_H

#include <gdk/graphics_webgles/build_info.h>

#if defined JFC_TARGET_PLATFORM_Emscripten || defined JFC_TARGET_PLATFORM_Android
// OpenGL ES 2: WebGL 1 in a browser, GLES 2 on Android and iOS. What differs from desktop GL (a shader's
// #version and precision, extensions to ask for) is said once, as GDK_GRAPHICS_GLES
#include <GLES2/gl2.h>
#define GDK_GRAPHICS_GLES 1
#elif defined JFC_TARGET_PLATFORM_iOS
#ifndef GLES_SILENCE_DEPRECATION
#define GLES_SILENCE_DEPRECATION
#endif
#include <OpenGLES/ES2/gl.h>
#define GDK_GRAPHICS_GLES 1
#elif defined JFC_TARGET_PLATFORM_Darwin
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#include <OpenGL/gl.h>
#elif defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows || defined JFC_TARGET_PLATFORM_FreeBSD
// desktop GL, its functions loaded by GLEW
#include <GL/glew.h>
#define GDK_GRAPHICS_GLEW 1
#else
#error the current platform is not supported
#endif

#ifndef GDK_GRAPHICS_GLES
#define GDK_GRAPHICS_GLES 0
#endif

#ifndef GDK_GRAPHICS_GLEW
#define GDK_GRAPHICS_GLEW 0
#endif

#endif
