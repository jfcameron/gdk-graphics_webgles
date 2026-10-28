// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_WEBGL1ES2_CONTEXT_H
#define GDK_GRAPHICS_WEBGL1ES2_CONTEXT_H

#include <gdk/graphics/context.h>
#include <gdk/graphics/profile.h>

#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
#endif

namespace gdk::graphics {
    class gl_state;

    /// \brief the gdk graphics interface over OpenGL ES 2.0, WebGL 1.0 and desktop OpenGL 2.0
    ///
    /// Every call needs the GL context it was made in current, on that thread.
    class webgl1es2_context final : public graphics::context {
    public:
        //! needs a current GL context. Throws gdk::graphics::exception when GL's functions cannot be loaded.
        [[nodiscard]] static std::shared_ptr<webgl1es2_context> make(std::optional<graphics::profile> aProfile = {});

        using graphics::context::make_material;
        using graphics::context::make_texture;

        /// \brief a shader program from GLSL ES 1.00 source, checked against the context's profile
        /// \throws gdk::graphics::exception past the profile, or when the program does not compile and link
        [[nodiscard]] shader_ptr_type make_shader(std::string_view aVertexSource, std::string_view aFragmentSource);

        //! what has been drawn
        struct counts final {
            std::size_t draws = 0;    //!< draw calls
            std::size_t programs = 0; //!< changes of shader program
        };

        //! what has been drawn since the last time this was asked
        [[nodiscard]] counts take_counts();

#ifdef __EMSCRIPTEN__
        /// \brief replaces a texture's contents with a JS image source: a VideoFrame, an ImageBitmap, an image or
        /// video element, a canvas. The browser draws it in on the GPU, converting as it goes; nothing is copied
        /// through the CPU.
        ///
        /// The texture takes the source's size. As that is seldom a power of two, filtering becomes linear without
        /// mipmaps and wrapping clamped, which is what WebGL 1 takes then.
        /// \warning on the thread that owns the WebGL context, where a VideoFrame also lives
        /// \throws gdk::graphics::exception the texture is not this context's, the source has no size, or it is
        /// past the profile's largest texture
        void update_from(texture &aTexture, const emscripten::val &aImageSource);
#endif

#if GDK_GRAPHICS_HAS_EGL
        //! an EGLImage, as EGL's own `EGLImage` type: a `void *`, so that EGL's headers are not this one's
        using egl_image_type = void *;

        /// \brief whether this context can sample an EGLImage: GL_OES_EGL_image is there, which needs the context
        /// to be EGL's (GLFW_EGL_CONTEXT_API on X11; Wayland's always is)
        [[nodiscard]] bool can_adopt_egl_images() const;

        /// \brief makes a texture's contents an EGLImage: the image's memory, sampled where it is, not copied. A
        /// hardware decoder's frame, say, imported from its DMA-BUF.
        ///
        /// Size as given; no mipmaps, and clamped. The image must outlive the texture's use of it: keep it, and
        /// what it was made from, until a later frame has replaced it on screen and the GPU has finished with it.
        /// A later update_data gives the texture storage of its own again.
        /// \throws gdk::graphics::exception the texture is not this context's, the context cannot adopt, or the
        /// image is null
        void adopt(texture &aTexture, egl_image_type apImage, std::size_t aWidth, std::size_t aHeight);
#endif

        [[nodiscard]] virtual scene_ptr_type make_scene() override;

        [[nodiscard]] virtual screen_camera_ptr_type make_camera() override;

        [[nodiscard]] virtual texture_camera_ptr_type make_texture_camera() override;
        [[nodiscard]] virtual texture_camera_ptr_type make_texture_camera(const intvector2_type &aSize) override;

        /// \throws gdk::graphics::exception the model or the material is not this context's
        [[nodiscard]] virtual entity_ptr_type make_entity(const const_model_ptr_type pModel,
            const const_material_ptr_type pMaterial) override;

        /// \throws gdk::graphics::exception the shader is not this context's
        [[nodiscard]] virtual material_ptr_type make_material(const const_shader_ptr_type pShader,
            const material::render_mode aRenderMode, const material::face_culling_mode aFaceCullingMode) override;

        /// \throws gdk::graphics::exception the prototype is not this context's
        [[nodiscard]] virtual material_ptr_type make_material(const const_material_ptr_type &aPrototype) override;

        [[nodiscard]] virtual model_ptr_type make_model() override;

        [[nodiscard]] virtual model_ptr_type make_model(const model::usage_hint aUsage,
            const model_data &aModelData) override;

        [[nodiscard]] virtual texture_ptr_type make_texture(const texture_data::view &aTextureData,
            const texture::wrap_mode aWrapModeU, const texture::wrap_mode aWrapModeV,
            const texture::filter_mode aFilterMode) override;

        [[nodiscard]] virtual texture_ptr_type make_texture() override;

        [[nodiscard]] virtual shader_ptr_type make_alpha_cutoff_shader() const override;

        [[nodiscard]] virtual shader_ptr_type make_alpha_blend_shader() const override;

        [[nodiscard]] virtual graphics::limits limits() const override;

        [[nodiscard]] virtual model_ptr_type make_cube_model() const override;

        [[nodiscard]] virtual model_ptr_type make_sphere_model() const override;

        webgl1es2_context(const webgl1es2_context &) = delete;
        webgl1es2_context &operator=(const webgl1es2_context &) = delete;

        virtual ~webgl1es2_context() override;

        /// \brief **what was drawn into the window, read back**: a rectangle of it from aX, aY, its bottom left, red,
        /// green, blue and alpha a byte each, a row at a time from the bottom. Before the buffers swap.
        [[nodiscard]] std::vector<std::byte> read_pixels(int aX, int aY, int aWidth, int aHeight) const;

    private:
        explicit webgl1es2_context(std::optional<graphics::profile> aProfile);

        [[nodiscard]] shader_ptr_type built_in(shader_ptr_type &aCached, std::string_view aVertexSource,
            std::string_view aFragmentSource) const;

        std::shared_ptr<gl_state> m_pState;

        graphics::limits m_Limits;

        std::shared_ptr<const graphics::profile> m_pProfile;

        mutable shader_ptr_type m_pAlphaCutoff;
        mutable shader_ptr_type m_pAlphaBlend;
    };
}

#endif
