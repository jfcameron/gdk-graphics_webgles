// © Joseph Cameron - MIT License

#include <gdk/graphics/webgl1es2_context.h>
#include <gdk/graphics/webgl1es2_profile.h>

int main() {
    using namespace gdk::graphics;

    auto *volatile make = &webgl1es2_context::make;

    return make != nullptr && webgl1_minimum().most.texture_size == 64 ? 0 : 1;
}
