#pragma once

#include <tuesday/gfx.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <chrono>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

using clock_type = std::chrono::high_resolution_clock;
using time_point = clock_type::time_point;
using delta_time = std::chrono::duration<float>;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline const char *vert_source = R"(
    #version 450

    layout (location = 0) in vec3 vPos;
    layout (location = 1) in vec3 vColor;
    layout (location = 2) in float vRadius;

    uniform mat4 MatP;
    uniform mat4 MatV;
    uniform mat4 MatM;

    layout (location = 0) out vec3 fPos;
    layout (location = 1) out vec3 fColor;
    layout (location = 2) out float gRadius;

    void main() {
        mat4 m = MatM;
        mat4 vm = MatV * m;

        fPos = (vm * vec4(vPos, 1)).xyz;
        fColor = vColor;
        gRadius = vRadius;

        gl_Position = MatP * vm * vec4(vPos, 1);
    }
)";

inline const char *vert_source_inst = R"(
    #version 450

    layout (location = 0) in vec3 vPos;
    layout (location = 1) in vec3 vOff;
    layout (location = 2) in vec3 vColor;

    uniform mat4 MatP;
    uniform mat4 MatV;
    uniform mat4 MatM;

    layout (location = 0) out vec3 fPos;
    layout (location = 1) out vec3 fColor;

    void main() {
        mat4 vm = MatV * MatM;
        vec3 pos = vPos + vOff;

        fPos = (vm * vec4(pos, 1)).xyz;
        fColor = vec3(vColor);

        gl_Position = MatP * vm * vec4(pos, 1);
    }
)";

inline const char *frag_source = R"(
    #version 450

    layout (location = 0) in vec3 fPos;
    layout (location = 1) in vec3 fColor;

    layout (location = 0) out vec4 fOutColor;

    void main() {
        fOutColor = vec4(fColor, 1);
    }
)";

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <> struct tue::gfx::attrib_format_fn<glm::u8vec3> {
    constexpr auto operator()() const noexcept {
        return attrib_format{
            .size = 3,
            .type = GL_UNSIGNED_BYTE,
            .normalized = GL_TRUE,
        };
    }
};

template <> struct tue::gfx::attrib_format_fn<glm::vec3> {
    constexpr auto operator()() const noexcept {
        return attrib_format{
            .size = 3,
            .type = GL_FLOAT,
            .normalized = GL_FALSE,
        };
    }
};

template <> struct tue::gfx::vertex_attrib_format_fn<glm::u8vec3> {
    constexpr auto operator()() const noexcept {
        return tue::gfx::vertex_attrib_format{
            .index = 0,
            .offset = 0,
            .attr = tue::gfx::attrib_format_for<glm::u8vec3>,
        };
    }
};

template <> struct tue::gfx::vertex_attrib_format_fn<glm::vec3> {
    constexpr auto operator()() const noexcept {
        return tue::gfx::vertex_attrib_format{
            .index = 0,
            .offset = 0,
            .attr = tue::gfx::attrib_format_for<glm::vec3>,
        };
    }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

constexpr auto cube_vertices(float r) noexcept {
    return std::array{
        // front
        glm::vec3{-r, -r, +r},
        glm::vec3{+r, -r, +r},
        glm::vec3{+r, +r, +r},
        glm::vec3{-r, +r, +r},
        // back
        glm::vec3{+r, -r, -r},
        glm::vec3{-r, -r, -r},
        glm::vec3{-r, +r, -r},
        glm::vec3{+r, +r, -r},
    };
}

constexpr auto cube_indices() noexcept {
    return std::array{
        0, 1, 2, 2, 3, 0, // front
        4, 5, 6, 6, 7, 4, // back
        5, 0, 3, 3, 6, 5, // left
        1, 4, 7, 7, 2, 1, // right
        3, 2, 7, 7, 6, 3, // top
        5, 4, 1, 1, 0, 5, // bottom
    };
}

constexpr auto cube_mesh(float r, glm::vec3 c = glm::vec3{0}) noexcept {
    const auto is = cube_indices();
    const auto vs = cube_vertices(r);
    std::array<glm::vec3, is.size()> m;
    for (auto i{0U}; i < is.size(); ++i) {
        m[i] = c + vs[is[i]];
    }
    return m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
