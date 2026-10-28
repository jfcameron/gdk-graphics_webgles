// © Joseph Cameron - MIT License

#include "conformance.h"
#include "test_include.h"

#include <gdk/graphics/webgl1es2_context.h>
#include <gdk/graphics/webgl1es2_profile.h>

#include <utility>

namespace gdk::graphics::conformance {
    context_ptr_type make_context(std::optional<profile> aProfile) {
        initGL();

        return webgl1es2_context::make(std::move(aProfile));
    }

    std::string_view implementation() { return "webgles"; }

    profile minimum_profile() { return webgl1_minimum(); }

    bool checks_shaders() { return true; }
}
