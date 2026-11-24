// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_WEBGL1ES2_VOLUME_TEXTURE_H
#define GDK_GRAPHICS_WEBGL1ES2_VOLUME_TEXTURE_H

#include <gdk/graphics/profile.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/volume_texture.h>
#include <gdk/graphics/webgl1es2_texture.h>

#include <array>
#include <cstddef>
#include <memory>
#include <optional>

namespace gdk::graphics {
    /// \brief a volume packed into a square 2D texture, its bytes in order: an edge L, a perfect square, packs into a
    /// texture L times sqrt(L) across, which `gdk_texture3D` and `gdk_volume` read
    class webgl1es2_volume_texture final : public volume_texture {
    public:
        /// \throws gdk::graphics::exception for a volume that is not a cube, an edge that is not a perfect square, or a
        /// packed texture past the device's or the profile's largest
        webgl1es2_volume_texture(const texture_data::volume_view &aView, wrap_mode aWrapModeU, wrap_mode aWrapModeV,
            wrap_mode aWrapModeW, filter_mode aFilterMode, std::shared_ptr<const profile> apProfile);

        virtual void update_data(const texture_data::volume_view &aView) override;

        virtual void update_data(const texture_data::volume_view &aView, std::size_t aOffsetX, std::size_t aOffsetY,
            std::size_t aOffsetZ) override;

        //! the 2D texture the volume is packed into
        [[nodiscard]] const webgl1es2_texture &packed() const;

        //! texels along each edge
        [[nodiscard]] std::size_t length() const;

        //! each axis' wrap mode for `gdk_volume`: 0 clamped, 1 repeat, 2 mirrored
        [[nodiscard]] vector3_type wrap() const;

        [[nodiscard]] bool smooth() const;

    private:
        std::shared_ptr<const profile> m_pProfile;
        std::array<wrap_mode, 3> m_Wrap;
        filter_mode m_Filter;
        std::size_t m_Length = 0;
        std::size_t m_Side = 0;
        std::optional<format> m_Format;
        std::unique_ptr<webgl1es2_texture> m_pPacked;
    };
}

#endif
