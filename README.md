## gdk-graphics_webgles

An implementation of [gdk-graphics](https://github.com/jfcameron/gdk-graphics) for OpenGL Embedded Systems 2.0 and WebGL 1.0.

Why GLES2 and WebGL1 specifically? Ubiquity. OpenGL ES 2.0 (2007) is supported by nearly every phone and set-top box from 2009 onward, and it’s closely based on desktop OpenGL 2.0 (2004), covering desktops and laptops from the mid-2000s. WebGL 1.0 (2011) is based on ES 2.0, so the differences are small and the same implementation reaches hardware-accelerated browsers from the early 2010s onward. Where there’s no GPU, or no access to one (VMs, for example), software implementations such as Mesa llvmpipe and SwiftShader provide ES 2.0. So this implementation runs, or can be made to run essentially anywhere.

The weakness of course is that it is a very old spec, so its feature set and resource limits are fairly restrictive. Those restrictions however just establish a floor. In the case of additional features, these are provided by OpenGL via extensions (float and depth textures, etc), and in the case of limits, the actual upper limits on a given device can be checked at runtime (max uniform vectors, max texture size, etc). To prevent exceeding limits, the library has the concept of "profiles". A profile lets you set limits ahead of time, so that if you exceed one, you'll see the problem on a development machine as an exception, rather than a graphical bug later down the road on a more restrictive end user device. The library provides a profile for ES 2's minimums, as well as WebGL 1's minimums, however its more practical to write your own profile to establish limits more inline with the hardware you're actually targeting. Extensions you simply have to be mindful of. If it's for some graphical effect, maybe provide an alternative implementation or disable it if the extension is not present. If it is absolutely necessary for the program to run, then detect it up front and give an error message explaining the missing requirement.

### Building

quickstart:

```
cmake --preset linux-clang
cmake --build --preset linux-clang
ctest --preset linux-clang
```

`CMakePresets.json` lists the supported builds. `linux-gcc-demo` builds the demos. `emscripten` builds them for the browser (`emcmake cmake --preset emscripten`). CI scripts are in `.github/workflows/ci.yml`, they cover Linux, Windows, Mac, Android, iOS and emscripten.
