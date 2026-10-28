// © Joseph Cameron - MIT License

#include <gdk/graphics/glh.h>
#include <gdk/graphics/exception.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_texture.h>
#include <gdk/graphics/webgl1es2_texture_camera.h>

#include <string>


using namespace gdk;
using namespace gdk::graphics;

namespace {
    void attach(const GLuint aFrameBuffer, const GLenum aAttachment, const webgl1es2_texture &aTexture,
        const char *aWhat) {
        glBindFramebuffer(GL_FRAMEBUFFER, aFrameBuffer);

        glFramebufferTexture2D(GL_FRAMEBUFFER, aAttachment, GL_TEXTURE_2D, aTexture.getHandle(), 0);

        const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (status != GL_FRAMEBUFFER_COMPLETE)
            throw exception(std::string("webgl1es2_texture_camera: the framebuffer is incomplete with its ")
                .append(aWhat).append(" attached"));
    }
}

webgl1es2_texture_camera::webgl1es2_texture_camera(const intvector2_type &aTextureSize,
    std::shared_ptr<const profile> aProfile)
: webgl1es2_camera()
, m_TextureSize(aTextureSize)
, m_pProfile(std::move(aProfile))
, m_FrameBufferHandle([]() {
    GLuint handle;
    glGenFramebuffers(1, &handle);

    return handle;
}(),
[](GLuint handle) {
    glDeleteFramebuffers(1, &handle);
})
, m_DepthBuffer([&]() {
    GLuint handle;
    glGenRenderbuffers(1, &handle);

    glBindRenderbuffer(GL_RENDERBUFFER, handle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
        static_cast<GLsizei>(aTextureSize.x), static_cast<GLsizei>(aTextureSize.y));
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferHandle.get());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, handle);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return handle;
}(),
[](GLuint handle) {
    glDeleteRenderbuffers(1, &handle);
})
, m_pColorTexture(std::make_shared<webgl1es2_texture>(texture_data::view{
        .width = static_cast<std::size_t>(aTextureSize.x),
        .height = static_cast<std::size_t>(aTextureSize.y),
        .format = texture::format::rgb,
        .data = nullptr},
    texture::wrap_mode::clamped, texture::wrap_mode::clamped,
    webgl1es2_texture::minification_filter::nearest, webgl1es2_texture::magnification_filter::nearest)) {
    m_pColorTexture->hold_to(m_pProfile);

    attach(m_FrameBufferHandle.get(), GL_COLOR_ATTACHMENT0, *m_pColorTexture, "colour texture");
}

void webgl1es2_texture_camera::set_perspective_projection(const float aFieldOfView,
    const float aNearClippingPlane, const float aFarClippingPlane,
    const float aViewportAspectRatio) {
    webgl1es2_camera::set_perspective_projection(aFieldOfView, aNearClippingPlane,
        aFarClippingPlane, aViewportAspectRatio);
}

void webgl1es2_texture_camera::set_perspective_projection(const float aFieldOfView,
    const float aNearClippingPlane, const float aFarClippingPlane,
    const float aViewportAspectRatio, const gdk::graphics::vector2_type &aShift) {
    webgl1es2_camera::set_perspective_projection(aFieldOfView, aNearClippingPlane,
        aFarClippingPlane, aViewportAspectRatio, aShift);
}

void webgl1es2_texture_camera::set_orthographic_projection(const gdk::graphics::vector2_type &aOrthographicSize,
    const float aNearClippingPlane, const float aFarClippingPlane,
    const float aViewportAspectRatio) {
    webgl1es2_camera::set_orthographic_projection(aOrthographicSize, aNearClippingPlane,
        aFarClippingPlane, aViewportAspectRatio);
}

void webgl1es2_texture_camera::set_clear_color(const gdk::graphics::color& acolor) {
    webgl1es2_camera::set_clear_color(acolor);
}

void webgl1es2_texture_camera::set_clear_mode(const clear_mode aClearMode) {
    webgl1es2_camera::set_clear_mode(aClearMode); 
}

void webgl1es2_texture_camera::set_transform(const gdk::graphics::vector3_type& aWorldPos, const gdk::graphics::quaternion_type& aRotation) {
    webgl1es2_camera::set_transform(aWorldPos, aRotation); 
}

void webgl1es2_texture_camera::set_transform(const gdk::graphics::matrix4x4_type &aMatrix) {
    webgl1es2_camera::set_transform(aMatrix);
}

texture_ptr_type webgl1es2_texture_camera::color_texture() const { return m_pColorTexture; }

texture_ptr_type webgl1es2_texture_camera::depth_texture() const {
    if (!m_pDepthTexture) {
        auto pDepth = std::make_shared<webgl1es2_texture>(webgl1es2_texture::format::depth,
            static_cast<std::size_t>(m_TextureSize.x), static_cast<std::size_t>(m_TextureSize.y), nullptr,
            texture::wrap_mode::clamped, texture::wrap_mode::clamped,
            webgl1es2_texture::minification_filter::nearest, webgl1es2_texture::magnification_filter::nearest);

        attach(m_FrameBufferHandle.get(), GL_DEPTH_ATTACHMENT, *pDepth, "depth texture");

        m_pDepthTexture = std::move(pDepth);
    }

    return m_pDepthTexture;
}

void webgl1es2_texture_camera::activate() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferHandle.get());

    glh::Viewport({0, 0}, m_TextureSize);
    glh::Scissor ({0, 0}, m_TextureSize);

    webgl1es2_camera::activate_clear_mode();
}

