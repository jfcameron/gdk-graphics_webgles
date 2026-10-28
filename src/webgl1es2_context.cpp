// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_context.h>
#include <gdk/graphics/webgl1es2_profile.h>

#include <gdk/graphics/opengl.h>
#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_gl_state.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_scene.h>
#include <gdk/graphics/webgl1es2_screen_camera.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/webgl1es2_texture.h>
#include <gdk/graphics/webgl1es2_texture_camera.h>

#include <cstring>
#include <string>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    void load_gl_functions() {
#if GDK_GRAPHICS_GLEW
        glewExperimental = GL_TRUE;

        const GLenum err = glewInit();

#if defined JFC_TARGET_PLATFORM_Linux
        const bool loaded = err == GLEW_OK || err == GLEW_ERROR_NO_GLX_DISPLAY;
#else
        const bool loaded = err == GLEW_OK;
#endif

        if (!loaded)
            throw exception(std::string("webgl1es2_context::make: glewInit failed, is a GL context current? ")
                .append(reinterpret_cast<const char *>(glewGetErrorString(err))));
#endif
    }

    [[nodiscard]] std::size_t gl_integer(const GLenum aWhich) {
        GLint value = 0;
        glGetIntegerv(aWhich, &value);

        return static_cast<std::size_t>(value < 0 ? 0 : value);
    }

    [[nodiscard]] graphics::limits limits_of_gl() {
        graphics::limits out;
        out.texture_size = static_cast<std::size_t>(webgl1es2_texture::getMaxTextureSize());
#if GDK_GRAPHICS_GLES
        out.non_power_of_two_textures = [] {
            const auto *const pExtensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

            return pExtensions && std::strstr(pExtensions, "GL_OES_texture_npot");
        }();
#else
        out.non_power_of_two_textures = true;
#endif
        out.texture_units = gl_integer(GL_MAX_TEXTURE_IMAGE_UNITS);
        out.vertex_texture_units = gl_integer(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
        out.vertex_attributes = gl_integer(GL_MAX_VERTEX_ATTRIBS);
#ifdef GL_MAX_VERTEX_UNIFORM_VECTORS
        out.vertex_uniform_vectors = gl_integer(GL_MAX_VERTEX_UNIFORM_VECTORS);
        out.fragment_uniform_vectors = gl_integer(GL_MAX_FRAGMENT_UNIFORM_VECTORS);
        out.varying_vectors = gl_integer(GL_MAX_VARYING_VECTORS);
#else
        out.vertex_uniform_vectors = gl_integer(GL_MAX_VERTEX_UNIFORM_COMPONENTS) / 4;
        out.fragment_uniform_vectors = gl_integer(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS) / 4;
        out.varying_vectors = gl_integer(GL_MAX_VARYING_FLOATS) / 4;
#endif
        return out;
    }

    template<typename own_type, typename given_type>
    [[nodiscard]] std::shared_ptr<own_type> own(const std::shared_ptr<given_type> &pGiven, const char *aWhat) {
        if (!pGiven) throw exception(std::string("webgl1es2_context: no ").append(aWhat));

        auto pOwn = std::dynamic_pointer_cast<own_type>(std::const_pointer_cast<std::remove_const_t<given_type>>(pGiven));

        if (!pOwn) throw exception(std::string("webgl1es2_context: the ").append(aWhat)
            .append(" was made by another kind of context"));

        return pOwn;
    }

    [[nodiscard, maybe_unused]] webgl1es2_texture &own(texture &aTexture) {
        auto *const pOwn = dynamic_cast<webgl1es2_texture *>(&aTexture);

        if (!pOwn) throw exception("webgl1es2_context: the texture was made by another kind of context");

        return *pOwn;
    }
}

std::shared_ptr<webgl1es2_context> webgl1es2_context::make(std::optional<graphics::profile> aProfile) {
    load_gl_functions();

    return std::shared_ptr<webgl1es2_context>(new webgl1es2_context(std::move(aProfile)));
}

webgl1es2_context::webgl1es2_context(std::optional<graphics::profile> aProfile)
: m_pState(std::make_shared<gl_state>())
, m_Limits(limits_of_gl())
, m_pProfile(aProfile ? std::make_shared<const graphics::profile>(std::move(*aProfile)) : nullptr) {
    m_pState->limit_texture_units(limits().texture_units);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
}

webgl1es2_context::~webgl1es2_context() = default;

webgl1es2_context::counts webgl1es2_context::take_counts() {
    const auto taken = m_pState->taken();

    return {taken.draws, taken.programs};
}

screen_camera_ptr_type webgl1es2_context::make_camera() {
    return std::make_shared<webgl1es2_screen_camera>();
}

texture_camera_ptr_type webgl1es2_context::make_texture_camera() {
    return make_texture_camera({1024, 1024});
}

texture_camera_ptr_type webgl1es2_context::make_texture_camera(const intvector2_type &aSize) {
    if (aSize.x <= 0 || aSize.y <= 0) throw exception("webgl1es2_context::make_texture_camera: a size must be positive");

    if (m_pProfile)
        check_texture(*m_pProfile, static_cast<std::size_t>(aSize.x), static_cast<std::size_t>(aSize.y), true);

    return std::make_shared<webgl1es2_texture_camera>(aSize, m_pProfile);
}

entity_ptr_type webgl1es2_context::make_entity(const const_model_ptr_type pModel,
    const const_material_ptr_type pMaterial) {
    return std::make_shared<webgl1es2_entity>(own<webgl1es2_model>(pModel, "model"),
        own<webgl1es2_material>(pMaterial, "material"));
}

material_ptr_type webgl1es2_context::make_material(const const_shader_ptr_type pShader,
    const material::render_mode aRenderMode, const material::face_culling_mode aFaceCullingMode) {
    return std::make_shared<webgl1es2_material>(own<webgl1es2_shader_program>(pShader, "shader"),
        aFaceCullingMode, aRenderMode);
}

material_ptr_type webgl1es2_context::make_material(const const_material_ptr_type &aPrototype) {
    return std::make_shared<webgl1es2_material>(*own<webgl1es2_material>(aPrototype, "prototype"));
}

shader_ptr_type webgl1es2_context::make_shader(const std::string_view aVertexSource,
    const std::string_view aFragmentSource) {
    if (m_pProfile) check_es2_shader(*m_pProfile, aVertexSource, aFragmentSource);

    return std::make_shared<webgl1es2_shader_program>(std::string(aVertexSource), std::string(aFragmentSource));
}

shader_ptr_type webgl1es2_context::built_in(shader_ptr_type &aCached, const std::string_view aVertexSource,
    const std::string_view aFragmentSource) const {
    if (!aCached) {
        if (m_pProfile) check_es2_shader(*m_pProfile, aVertexSource, aFragmentSource);

        aCached = std::make_shared<webgl1es2_shader_program>(std::string(aVertexSource), std::string(aFragmentSource));
    }

    return aCached;
}

std::vector<std::byte> webgl1es2_context::read_pixels(const int aX, const int aY, const int aWidth,
    const int aHeight) const {
    if (aWidth <= 0 || aHeight <= 0) return {};

    std::vector<std::byte> out(static_cast<std::size_t>(aWidth) * static_cast<std::size_t>(aHeight) * 4);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(aX, aY, aWidth, aHeight, GL_RGBA, GL_UNSIGNED_BYTE, out.data());

    return out;
}

graphics::limits webgl1es2_context::limits() const {
    return m_pProfile ? narrowest(m_Limits, m_pProfile->most) : m_Limits;
}

shader_ptr_type webgl1es2_context::make_alpha_blend_shader() const {
    return built_in(m_pAlphaBlend, webgl1es2_shader_program::ALPHA_BLEND_VERTEX,
        webgl1es2_shader_program::ALPHA_BLEND_FRAGMENT);
}

shader_ptr_type webgl1es2_context::make_alpha_cutoff_shader() const {
    return built_in(m_pAlphaCutoff, webgl1es2_shader_program::ALPHA_CUTOFF_VERTEX,
        webgl1es2_shader_program::ALPHA_CUTOFF_FRAGMENT);
}

model_ptr_type webgl1es2_context::make_cube_model() const {
    const auto pModel = webgl1es2_model::make_cube();
    pModel->hold_to(m_pProfile);

    return pModel;
}

model_ptr_type webgl1es2_context::make_sphere_model() const {
    const auto pModel = webgl1es2_model::make_sphere();
    pModel->hold_to(m_pProfile);

    return pModel;
}

texture_ptr_type webgl1es2_context::make_texture(const texture_data::view &aTextureData,
    const texture::wrap_mode aWrapModeU, const texture::wrap_mode aWrapModeV, const texture::filter_mode aFilterMode) {
    if (m_pProfile) check_texture(*m_pProfile, aTextureData.width, aTextureData.height,
        aWrapModeU == texture::wrap_mode::clamped && aWrapModeV == texture::wrap_mode::clamped);

    const auto sharp = aFilterMode == texture::filter_mode::sharp;

    const auto pTexture = std::make_shared<webgl1es2_texture>(aTextureData, aWrapModeU, aWrapModeV,
        sharp ? webgl1es2_texture::minification_filter::nearest : webgl1es2_texture::minification_filter::linear,
        sharp ? webgl1es2_texture::magnification_filter::nearest : webgl1es2_texture::magnification_filter::linear);
    pTexture->hold_to(m_pProfile);

    return pTexture;
}

texture_ptr_type webgl1es2_context::make_texture() {
    const texture_data::view view{.width = 0, .height = 0, .format = texture::format::rgba, .data = nullptr};

    const auto pTexture = std::make_shared<webgl1es2_texture>(view);
    pTexture->hold_to(m_pProfile);

    return pTexture;
}

model_ptr_type webgl1es2_context::make_model(const model::usage_hint aUsage, const model_data &aModelData) {
    if (m_pProfile) check_model(*m_pProfile, aModelData);

    const auto pModel = std::make_shared<webgl1es2_model>(aUsage, aModelData);
    pModel->hold_to(m_pProfile);

    return pModel;
}

model_ptr_type webgl1es2_context::make_model() {
    return make_model(model::usage_hint::upload_once, model_data());
}

scene_ptr_type webgl1es2_context::make_scene() {
    return std::make_shared<webgl1es2_scene>(m_pState);
}

#ifdef __EMSCRIPTEN__
void webgl1es2_context::update_from(texture &aTexture, const emscripten::val &aImageSource) {
    own(aTexture).update_data(aImageSource);
}
#endif

#if GDK_GRAPHICS_HAS_EGL
bool webgl1es2_context::can_adopt_egl_images() const { return webgl1es2_texture::can_adopt_egl_images(); }

void webgl1es2_context::adopt(texture &aTexture, const egl_image_type apImage, const std::size_t aWidth,
    const std::size_t aHeight) {
    own(aTexture).adopt(apImage, aWidth, aHeight);
}
#endif
