#ifndef _TUE_GFX_SHADER_HPP_INCLUDED_
#define _TUE_GFX_SHADER_HPP_INCLUDED_

#include <tuesday/assert.hpp>
#include <tuesday/gfx/gl.hpp>

#include <span>
#include <string_view>

namespace tue::gfx {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct shader_stage {
    GLuint id{0};
    GLenum type{GL_NONE};

    explicit constexpr operator bool() const noexcept { return id > 0; }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct shader_program {
    GLuint id{0};

    explicit constexpr operator bool() const noexcept { return id > 0; }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

shader_stage make_shader_stage(std::string_view src, GLenum type) noexcept;

void delete_shader_stage(shader_stage &s) noexcept;

shader_program link_shader(std::span<shader_stage> stages) noexcept;

shader_program make_shader(std::string_view vs_src,
                           std::string_view fs_src) noexcept;

void delete_shader(shader_program &s) noexcept;

GLint find_uniform(shader_program p, std::string_view name) noexcept;

template <typename T>
void bind_uniform(shader_program p, GLint location, T &&value) = delete;

// void bind_uniform(shader_program p, GLint location, GLint value) {
//     glUniform1i(location, value);
// }
//
// void bind_uniform(shader_program p, GLint location, GLfloat value) {
//     glUniform1f(location, value);
// }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename T>
void bind_uniform(shader_program p, std::string_view name, T &&value) {
    GLint loc = find_uniform(p, name);
    tue_assert(loc >= 0);
    bind_uniform(p, loc, std::forward<T>(value));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::gfx

#endif
