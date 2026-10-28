// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_WEBGL1ES2_TEXTURE_CAMERA_H
#define GDK_GRAPHICS_WEBGL1ES2_TEXTURE_CAMERA_H

#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/texture_camera.h>
#include <gdk/graphics/webgl1es2_camera.h>
#include <gdk/graphics/webgl1es2_texture.h>

#include <jfc/unique_handle.h>

#include <memory>
#include <optional>
#include <vector>

namespace gdk::graphics {
    class entity;

    /// \brief webgl1es2_camera implementation of texture_camera
    class webgl1es2_texture_camera final : public texture_camera, public webgl1es2_camera {
    public:
    /// \name external interface
    ///@{
    //

        virtual void set_perspective_projection(const float aFieldOfView,
            const float aNearClippingPlane, const float aFarClippingPlane,
            const float aViewportAspectRatio) override;

        virtual void set_perspective_projection(const float aFieldOfView,
            const float aNearClippingPlane, const float aFarClippingPlane,
            const float aViewportAspectRatio, const gdk::graphics::vector2_type &aShift) override;

        virtual void set_orthographic_projection(const gdk::graphics::vector2_type &aOrthographicSize,
            const float aNearClippingPlane, const float aFarClippingPlane,
            const float aViewportAspectRatio) override;

        virtual void set_clear_color(const gdk::graphics::color& acolor) override;

        virtual void set_clear_mode(const clear_mode aClearMode) override;

        virtual void set_transform(const gdk::graphics::vector3_type& aWorldPos, 
            const gdk::graphics::quaternion_type& aRotation) override;
        virtual void set_transform(const gdk::graphics::matrix4x4_type &aMatrix) override;
        
        [[nodiscard]] virtual texture_ptr_type color_texture() const override;

        /// \throws gdk::graphics::exception where GL ES 2 or WebGL 1 has no depth texture extension
        [[nodiscard]] virtual texture_ptr_type depth_texture() const override;
    ///@}
            
    /// \name internal interface
    ///@{
    //
        void activate() const;
    ///@}
        
        /// \param aProfile what its textures are held to, or null
        /// \throws gdk::graphics::exception where the framebuffer cannot be completed
        explicit webgl1es2_texture_camera(const intvector2_type &aTextureSize = {1024, 1024},
            std::shared_ptr<const profile> aProfile = {});

        ~webgl1es2_texture_camera() = default;

    private:
        intvector2_type m_TextureSize;

        std::shared_ptr<const profile> m_pProfile;

        //! handle to an FBO in the context
        jfc::unique_handle<GLuint> m_FrameBufferHandle;
        
        //! handle to the depth buffer
        jfc::unique_handle<GLuint> m_DepthBuffer;

        //! Texture attached to the color buffer
        std::shared_ptr<webgl1es2_texture> m_pColorTexture;

        //! Texture attached to the depth buffer, in place of the depth buffer, once asked for
        mutable std::shared_ptr<webgl1es2_texture> m_pDepthTexture;
    };
}

#endif

