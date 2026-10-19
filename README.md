## gdk-graphics_webgles

An implementation of [gdk-graphics](https://github.com/jfcameron/gdk-graphics) using OpenGL ES 2 and WebGL 1.

It runs wherever that subset of GL does: WebGL 1 in a browser (Emscripten), GLES 2 on Android, and desktop GL on Linux,
Windows and macOS.

## Building

```
cmake --preset linux-clang
cmake --build --preset linux-clang
ctest --preset linux-clang
```

`CMakePresets.json` lists the supported builds. `linux-gcc-demo` builds the demos, `emscripten` builds them for the
browser (`emcmake cmake --preset emscripten`).

