## gdk-graphics_webgles

An implementation of [gdk-graphics](https://github.com/jfcameron/gdk-graphics) for OpenGL Embedded Systems 2.0 and WebGL 1.0.

Why GLES2 and WebGL1 specifically? Ubiquity. OpenGL ES 2.0 (2007) is supported by nearly every phone and set-top box from 2009 onward, and it’s closely based on desktop OpenGL 2.0 (2004), covering desktops and laptops from the mid-2000s. WebGL 1.0 (2011) is based on ES 2.0, so the differences are small and the same implementation reaches hardware-accelerated browsers from the early 2010s onward. Where there’s no GPU, or no access to one (VMs, for example), software implementations such as Mesa llvmpipe and SwiftShader provide ES 2.0.

### Building

quickstart:

```
cmake --preset linux-clang
cmake --build --preset linux-clang
ctest --preset linux-clang
```

`CMakePresets.json` lists the supported builds. `linux-gcc-demo` builds the demos. `emscripten` builds them for the browser (`emcmake cmake --preset emscripten`). CI scripts are in `.github/workflows/ci.yml`, they cover Linux, Windows, Mac, Android, iOS and emscripten.
