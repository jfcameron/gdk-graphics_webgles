// © Joseph Cameron - MIT License

#include <gdk/graphics/glh.h>
#include <gdk/graphics/webgl1es2_screen_camera.h>
#include <gdk/graphics/webgl1es2_texture.h>

#include <cmath>
#include <utility>

using namespace gdk;
using namespace gdk::graphics;

webgl1es2_screen_camera::webgl1es2_screen_camera() = default;

void webgl1es2_screen_camera::set_viewport(const float aX, const float aY, 
    const float aWidth, const float aHeight) {
    m_ViewportPosition.x = aX;
    m_ViewportPosition.y = aY;

    m_ViewportSize.x = aWidth;
    m_ViewportSize.y = aHeight;
}

void webgl1es2_screen_camera::set_scissor(const float aX, const float aY, const float aWidth,
    const float aHeight) {
    m_Scissor = std::pair{vector2_type(aX, aY), vector2_type(aWidth, aHeight)};
}

void webgl1es2_screen_camera::clear_scissor() { m_Scissor.reset(); }

void webgl1es2_screen_camera::activate(const gdk::graphics::intvector2_type &aFrameBufferSize) const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const auto edge = [](const float aFraction, const signed_integral_type aPixels) {
        return static_cast<signed_integral_type>(std::lround(aFraction * static_cast<float>(aPixels)));
    };

    const auto in_pixels = [&](const vector2_type &aPosition, const vector2_type &aSize) {
        const intvector2_type from(edge(aPosition.x, aFrameBufferSize.x), edge(aPosition.y, aFrameBufferSize.y));
        const intvector2_type to(edge(aPosition.x + aSize.x, aFrameBufferSize.x),
            edge(aPosition.y + aSize.y, aFrameBufferSize.y));

        return std::pair{from, intvector2_type(to.x - from.x, to.y - from.y)};
    };

    const auto [viewportPosition, viewportSize] = in_pixels(m_ViewportPosition, m_ViewportSize);

    glh::Viewport(viewportPosition, viewportSize);

    if (m_Scissor) {
        const auto [scissorPosition, scissorSize] = in_pixels(m_Scissor->first, m_Scissor->second);

        glh::Scissor(scissorPosition, scissorSize);
    }
    else glh::Scissor(viewportPosition, viewportSize);
    
    webgl1es2_camera::activate_clear_mode();
}

void webgl1es2_screen_camera::set_perspective_projection(const float aFieldOfView,
    const float aNearClippingPlane, const float aFarClippingPlane,
    const float aViewportAspectRatio) {
    webgl1es2_camera::set_perspective_projection(aFieldOfView, aNearClippingPlane,
        aFarClippingPlane, aViewportAspectRatio);
}

void webgl1es2_screen_camera::set_perspective_projection(const float aFieldOfView,
    const float aNearClippingPlane, const float aFarClippingPlane,
    const float aViewportAspectRatio, const gdk::graphics::vector2_type &aShift) {
    webgl1es2_camera::set_perspective_projection(aFieldOfView, aNearClippingPlane,
        aFarClippingPlane, aViewportAspectRatio, aShift);
}

void webgl1es2_screen_camera::set_orthographic_projection(const gdk::graphics::vector2_type &aOrthographicSize,
    const float aNearClippingPlane, const float aFarClippingPlane,
    const float aViewportAspectRatio) {
    webgl1es2_camera::set_orthographic_projection(aOrthographicSize, aNearClippingPlane,
        aFarClippingPlane, aViewportAspectRatio);
}

void webgl1es2_screen_camera::set_clear_color(const gdk::graphics::color& acolor) {
     webgl1es2_camera::set_clear_color(acolor);
}

void webgl1es2_screen_camera::set_clear_mode(const gdk::graphics::camera::clear_mode aClearMode) {
    webgl1es2_camera::set_clear_mode(aClearMode);
}

void webgl1es2_screen_camera::set_transform(const gdk::graphics::vector3_type &aWorldPos, 
    const gdk::graphics::quaternion_type &aRotation) {
    webgl1es2_camera::set_transform(aWorldPos, aRotation);
}

void webgl1es2_screen_camera::set_transform(const gdk::graphics::matrix4x4_type &aMatrix) {
    webgl1es2_camera::set_transform(aMatrix);
}

