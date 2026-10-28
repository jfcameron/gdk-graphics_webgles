# gdk-graphics_webgles
An implementation of [gdk-graphics](https://github.com/jfcameron/gdk-graphics) using OpenGL Embedded Systems 2.0 and WebGL 1.0, and desktop OpenGL 2.0 where neither is native.

API documentation: https://jfcameron.github.io/gdk-graphics_webgles/

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler. On Linux, the tests and demos also need X11 or Wayland development files, as GLFW does.
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the library, its tests and its demos on linux using gcc
cmake --preset linux-gcc

# build them
cmake --build --preset linux-gcc

# run the tests: they open a window, so give them a display
ctest --preset linux-gcc
```
Without a display, `xvfb-run -a ctest --preset linux-gcc` runs them on a virtual one, with Mesa's software rasteriser. See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see how CI runners build it on different platforms.

To use it from another project, add it with `add_subdirectory` and link `gdk::graphics_webgles`, or install it and `find_package(gdk-graphics_webgles)`. `gdk::graphics_webgles_profile` alone gives the ES 2 and WebGL 1 profiles and the shader check, without GL.

## Using it
`webgl1es2_context::make` makes a context in the GL context current on the calling thread. Everything made from it is used there, with that context current: nothing is thread-safe.

Beyond the interface, the context offers:
- `make_shader`, from GLSL ES 1.00 source, checked against the context's profile;
- `take_counts`, the draws and program changes since it was last asked;
- `update_from`, in the browser, which draws a VideoFrame, an image or a canvas into a texture on the GPU;
- `adopt`, where GL's context is EGL's, which samples an EGLImage, a hardware decoder's frame say, in place.

On desktop GL, shaders are GLSL 1.10, with the ES precision qualifiers defined away: a context of the core profile cannot run them. Ask GLFW, or whatever makes the window, for a compatibility context, which is its default.

## Why GLES2 and WebGL1 specifically?
Ubiquity. OpenGL ES 2.0 (2007) is supported by nearly every phone and set-top box from 2009 onward, and it’s closely based on desktop OpenGL 2.0 (2004), covering desktops and laptops from the mid-2000s. WebGL 1.0 (2011) is based on ES 2.0, so the differences are small and the same implementation reaches hardware-accelerated browsers from the early 2010s onward. Where there’s no GPU, or no access to one (VMs, for example), software implementations such as Mesa llvmpipe and SwiftShader provide ES 2.0. So this implementation runs, or can be made to run essentially anywhere.

The weakness of course is that it is a very old spec, so its feature set and resource limits are fairly restrictive. Those restrictions however just establish a floor. In the case of additional features, these are provided by OpenGL via extensions (float and depth textures, etc), and in the case of limits, the actual upper limits on a given device can be checked at runtime (max uniform vectors, max texture size, etc). To prevent exceeding limits, the library has the concept of "profiles". A profile lets you set limits ahead of time, so that if you exceed one, you'll see the problem on a development machine as an exception, rather than a graphical bug later down the road on a more restrictive end user device. The library provides a profile for ES 2's minimums, as well as WebGL 1's minimums, however its more practical to write your own profile to establish limits more inline with the hardware you're actually targeting. Extensions you simply have to be mindful of. If it's for some graphical effect, maybe provide an alternative implementation or disable it if the extension is not present. If it is absolutely necessary for the program to run, then detect it up front and give an error message explaining the missing requirement.
