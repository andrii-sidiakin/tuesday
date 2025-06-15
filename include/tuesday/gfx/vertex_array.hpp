#ifndef _TUE_GFX_VERTEX_ARRAY_HPP_INCLUDED_
#define _TUE_GFX_VERTEX_ARRAY_HPP_INCLUDED_

#include <tuesday/assert.hpp>
#include <tuesday/gfx/gl.hpp>

#include <iterator>
#include <span>

namespace tue::gfx {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
struct attrib_format {
    GLint size{0};
    GLenum type{GL_NONE};
    GLboolean normalized{GL_FALSE};
};

///
template <typename T> struct attrib_format_fn;
///
template <typename T>
static constexpr auto attrib_format_for = attrib_format_fn<T>{}();

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
struct vertex_attrib_format {
    GLuint index{0};
    GLuint offset{0};
    attrib_format attr{};
};

///
template <typename T> struct vertex_attrib_format_fn;
///
template <typename T>
static constexpr auto vertex_attrib_format_for = vertex_attrib_format_fn<T>{}();

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
struct vertex_buffer {
    GLuint id{0};
    GLsizei stride{0};
    GLintptr count{0};

    explicit constexpr operator bool() const noexcept { return id != 0; }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
struct vertex_array {
    GLuint id{0};

    explicit constexpr operator bool() const noexcept { return id != 0; }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline vertex_buffer create_vertex_buffer() {
    GLuint id{0};
    glCreateBuffers(1, &id);
    tue_assert(id != 0);
    return vertex_buffer{.id = id};
}

template <class Elem>
inline vertex_buffer create_vertex_buffer_for(GLintptr count, GLenum usage) {
    static_assert(std::is_trivially_copyable_v<Elem>,
                  "Must be trivially copyable");
    auto vbo = create_vertex_buffer();
    vbo.count = count;
    vbo.stride = sizeof(Elem);
    glNamedBufferData(vbo.id, vbo.count * vbo.stride, nullptr, usage);
    return vbo;
}

inline vertex_buffer create_vertex_buffer(std::contiguous_iterator auto first,
                                          std::contiguous_iterator auto last,
                                          GLenum usage) {
    using elem_type = std::iterator_traits<decltype(first)>::value_type;
    static_assert(std::is_trivially_copyable_v<elem_type>,
                  "Must be trivially copyable");

    auto vbo = create_vertex_buffer();
    vbo.stride = sizeof(elem_type);
    vbo.count = std::distance(first, last);
    glNamedBufferData(vbo.id, vbo.count * vbo.stride, std::addressof(*first),
                      usage);
    return vbo;
}

inline vertex_buffer create_vertex_buffer(std::ranges::contiguous_range auto r,
                                          GLenum usage) {
    return create_vertex_buffer(std::ranges::begin(r), std::ranges::end(r),
                                usage);
}

inline void delete_vertex_buffer(vertex_buffer &vbo) {
    if (vbo) {
        glDeleteBuffers(1, &vbo.id);
    }
    vbo = vertex_buffer{};
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline vertex_array create_vertex_array() {
    GLuint id{0};
    glCreateVertexArrays(1, &id);
    tue_assert(id != 0);
    return vertex_array{.id = id};
}

inline void delete_vertex_array(vertex_array &vao) {
    if (vao) {
        glDeleteVertexArrays(1, &vao.id);
    }
    vao = vertex_array{};
}

inline void bind_attrib(vertex_array vao, GLuint binding_index,
                        vertex_attrib_format fmt) {
    tue_assert(vao.id != 0);
    tue_assert(binding_index < GL_MAX_VERTEX_ATTRIB_BINDINGS);
    tue_assert(fmt.index < GL_MAX_VERTEX_ATTRIBS);
    tue_assert(fmt.offset < GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET);
    tue_assert(fmt.attr.size > 0);

    glVertexArrayAttribFormat(vao.id, fmt.index, fmt.attr.size, fmt.attr.type,
                              fmt.attr.normalized, fmt.offset);
    glVertexArrayAttribBinding(vao.id, fmt.index, binding_index);
    glEnableVertexArrayAttrib(vao.id, fmt.index);
}

inline void bind_attrib(vertex_array vao, GLuint binding_index,
                        vertex_attrib_format fmt, GLuint divisor) {
    bind_attrib(vao, binding_index, fmt);
    glVertexArrayBindingDivisor(vao.id, binding_index, divisor);
}

inline void bind_attrib(vertex_array vao, GLuint binding_index,
                        std::span<const vertex_attrib_format> fmt) {
    for (auto elem : fmt) {
        bind_attrib(vao, binding_index, elem);
    }
}

inline bool bind_buffer(vertex_array vao, GLuint binding_index,
                        vertex_buffer vbo) {
    tue_assert(vao.id != 0);
    tue_assert(binding_index < GL_MAX_VERTEX_ATTRIB_BINDINGS);
    tue_assert(vbo.id != 0);
    tue_assert(vbo.stride > 0);

    const GLintptr offset = 0;
    glVertexArrayVertexBuffer(vao.id, binding_index, vbo.id, offset,
                              vbo.stride);
    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// inline void draw_object(vertex_array vao) {
//     tue_assert(vao.id != 0);
//     glBindVertexArray(vao.id);
// }
//
// inline void draw_object(vertex_buffer vbo, GLenum mode = GL_POINTS) {
//     tue_assert(vbo.id != 0);
//     glDrawArrays(mode, 0, vbo.count);
// }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::gfx

#endif
