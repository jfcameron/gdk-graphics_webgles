// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_WEBGL1ES2_GL_STATE_H
#define GDK_GRAPHICS_WEBGL1ES2_GL_STATE_H

#include <gdk/graphics/exception.h>
#include <gdk/graphics/opengl.h>
#include <gdk/graphics/string_keyed.h>

#include <cstddef>

#include <string>
#include <string_view>
#include <unordered_map>

namespace gdk::graphics {
    /// \brief what a context last told gl, so a call that changes nothing is skipped
    class gl_state final {
    public:
        /// \brief make aHandle the program in use if it is not already
        bool use_program(const GLuint aHandle) {
            if (mCurrentProgram == aHandle) return false;

            mCurrentProgram = aHandle;
            ++mCounted.programs;

            mTextureUnits.clear();
            mTextureUnitCounter = 0;

            glUseProgram(aHandle);

            return true;
        }

        [[nodiscard]] GLuint current_program() const { return mCurrentProgram; }

        [[nodiscard]] GLint assigned_texture_unit(const std::string_view aName) const {
            const auto found = mTextureUnits.find(aName);

            return found == mTextureUnits.end() ? -1 : found->second;
        }

        //! \throws gdk::graphics::exception past the context's texture units
        [[nodiscard]] GLint assign_texture_unit(const std::string_view aName) {
            if (static_cast<std::size_t>(mTextureUnitCounter) >= mTextureUnitLimit)
                throw exception("there are " + std::to_string(mTextureUnitLimit)
                    + " texture units, and a material is binding more textures than that: "
                    + std::string(aName));

            const GLint unit = mTextureUnitCounter++;

            mTextureUnits.emplace(aName, unit);

            return unit;
        }

        [[nodiscard]] GLint assigned_texture_unit_count() const { return mTextureUnitCounter; }

        //! each material's textures take units from the first: a material binds every texture it has as it is
        //! activated, so what an earlier one bound is not this one's business
        void begin_material() {
            mTextureUnits.clear();
            mTextureUnitCounter = 0;
        }

        void limit_texture_units(const std::size_t aLimit) { mTextureUnitLimit = aLimit; }

        [[nodiscard]] std::size_t texture_unit_limit() const { return mTextureUnitLimit; }

        struct counted final {
            std::size_t draws = 0;
            std::size_t programs = 0;
        };

        void count_draw() { ++mCounted.draws; }

        [[nodiscard]] counted taken() { const auto out = mCounted; mCounted = {}; return out; }

        void invalidate() {
            mCurrentProgram = 0;
            mTextureUnits.clear();
            mTextureUnitCounter = 0;
        }

    private:
        GLuint mCurrentProgram{0};
        string_keyed<GLint> mTextureUnits;
        GLint mTextureUnitCounter{0};
        std::size_t mTextureUnitLimit{8};
        counted mCounted;
    };
}

#endif
