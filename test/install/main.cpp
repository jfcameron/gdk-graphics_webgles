// © Joseph Cameron - MIT License

#include <gdk/graphics/webgl1es2_context.h>
#include <gdk/graphics/webgl1es2_profile.h>
#include <gdk/graphics/ext/webgl1es2_skinning.h>

int main() {
    using namespace gdk::graphics;

    auto *volatile make = &webgl1es2_context::make;

    return make != nullptr && webgl1_minimum().most.texture_size == 64 && ext::es2_most_bones(webgl1_minimum(), 8) == 40
        ? 0 : 1;
}
