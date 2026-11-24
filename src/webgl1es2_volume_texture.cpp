// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/texture_patch.h>
#include <gdk/graphics/webgl1es2_volume_texture.h>

#include <string>

using namespace gdk::graphics;

namespace {
    [[nodiscard]] std::size_t channels_of(const texture_data::format aFormat) {
        switch (aFormat) {
            case texture_data::format::grey: return 1;
            case texture_data::format::rg: return 2;
            case texture_data::format::rgb: return 3;
            case texture_data::format::rgba: return 4;
        }

        return 0;
    }

    [[nodiscard]] std::size_t whole_root_of(const std::size_t aNumber) {
        std::size_t root = 0;

        while ((root + 1) * (root + 1) <= aNumber) ++root;

        return root * root == aNumber ? root : 0;
    }

    [[nodiscard]] std::size_t packed_side(const texture_data::volume_view &aView,
        const std::shared_ptr<const profile> &apProfile) {
        const auto edge = aView.width;

        if (!edge || aView.height != edge || aView.depth != edge)
            throw exception("webgl1es2_volume_texture: a volume is packed into a square 2D texture, so it must be a "
                "cube with texels: " + std::to_string(aView.width) + " by " + std::to_string(aView.height) + " by "
                + std::to_string(aView.depth) + " is not");

        const auto root = whole_root_of(edge);

        if (!root)
            throw exception("webgl1es2_volume_texture: a cube's edge must be a perfect square (4, 9, 16, 25...) for "
                "it to pack into a square 2D texture: " + std::to_string(edge) + " is not");

        const auto side = edge * root;

        if (apProfile) check_texture(*apProfile, side, side, true);

        return side;
    }

    [[nodiscard]] texture::wrap_mode clamped() { return texture::wrap_mode::clamped; }

    [[nodiscard]] float wrap_code(const texture::wrap_mode aMode) {
        switch (aMode) {
            case texture::wrap_mode::clamped: return 0;
            case texture::wrap_mode::repeat: return 1;
            case texture::wrap_mode::mirrored: return 2;
        }

        return 0;
    }
}

webgl1es2_volume_texture::webgl1es2_volume_texture(const texture_data::volume_view &aView, const wrap_mode aWrapModeU,
    const wrap_mode aWrapModeV, const wrap_mode aWrapModeW, const filter_mode aFilterMode,
    std::shared_ptr<const profile> apProfile)
: m_pProfile(std::move(apProfile))
, m_Wrap{aWrapModeU, aWrapModeV, aWrapModeW}
, m_Filter(aFilterMode)
, m_Length(aView.width)
, m_Side(packed_side(aView, m_pProfile))
, m_Format(aView.format)
, m_pPacked(std::make_unique<webgl1es2_texture>(texture_data::view{.width = m_Side, .height = m_Side,
    .format = aView.format, .data = aView.data}, clamped(), clamped()))
{}

void webgl1es2_volume_texture::update_data(const texture_data::volume_view &aView) {
    const auto side = packed_side(aView, m_pProfile);

    m_pPacked->update_data({.width = side, .height = side, .format = aView.format, .data = aView.data});

    m_Length = aView.width;
    m_Side = side;
    m_Format = aView.format;
}

void webgl1es2_volume_texture::update_data(const texture_data::volume_view &aView, const std::size_t aOffsetX,
    const std::size_t aOffsetY, const std::size_t aOffsetZ) {
    check_patch(aView, aOffsetX, aOffsetY, aOffsetZ, m_Length, m_Length, m_Length, m_Format);

    if (!aView.data || !aView.width) return;

    const auto channels = channels_of(aView.format);

    for (std::size_t slice = 0; slice < aView.depth; ++slice)
        for (std::size_t row = 0; row < aView.height; ++row) {
            const auto first = aOffsetX + m_Length * (aOffsetY + row) + m_Length * m_Length * (aOffsetZ + slice);

            m_pPacked->update_data({.width = aView.width, .height = 1, .format = aView.format,
                .data = aView.data + (slice * aView.height + row) * aView.width * channels},
                first % m_Side, first / m_Side);
        }
}

const webgl1es2_texture &webgl1es2_volume_texture::packed() const { return *m_pPacked; }

std::size_t webgl1es2_volume_texture::length() const { return m_Length; }

vector3_type webgl1es2_volume_texture::wrap() const {
    return {wrap_code(m_Wrap[0]), wrap_code(m_Wrap[1]), wrap_code(m_Wrap[2])};
}

bool webgl1es2_volume_texture::smooth() const { return m_Filter == filter_mode::smooth; }
