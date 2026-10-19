// © Joseph Cameron - All Rights Reserved

/// \file
/// \brief **the conformance suite over OpenGL ES 2**: what the interface promises, held of gdkgraphics_webgles on a
/// window's GL context, as of the null context and any other. \see gdk-graphics' test/conformance/conformance.h

#include "conformance.h"
#include "test_include.h"

#include <gdk/graphics/webgl1es2_context.h>

#include <utility>

namespace gdk::graphics::conformance {
    context_ptr_type make_context(std::optional<profile> aProfile) {
        initGL();

        return webgl1es2_context::make(std::move(aProfile));
    }

    std::string_view implementation() { return "webgles"; }
}
