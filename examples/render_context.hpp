#pragma once

#include "helpers.hpp"

namespace tue::gfx {

struct draw_context {
    float width{0};
    float height{0};
    float aspect_ratio{0};

    glm::mat4 mat_p{1};
    glm::mat4 mat_v{1};
    glm::mat4 mat_m{1};

    tue::gfx::shader_program m_shader;
    tue::gfx::vertex_array m_vao;

    void set_viewport(int width_, int height_) {
        if (tue_assert(width_ > 0 && height_ > 0)) {
            width = width_;
            height = height_;
            aspect_ratio = width / height;
            glViewport(0, 0, width_, height_);
        }
        else {
            width = height = aspect_ratio = 0;
        }
    }

    void use(tue::gfx::shader_program shader) {
        if (m_shader.id != shader.id) {
            glUseProgram(shader.id);
            m_shader = shader;
        }

        glUniformMatrix4fv(find_uniform(shader, "MatP"), 1, GL_FALSE,
                           glm::value_ptr(mat_p));
        glUniformMatrix4fv(find_uniform(shader, "MatV"), 1, GL_FALSE,
                           glm::value_ptr(mat_v));
        glUniformMatrix4fv(find_uniform(shader, "MatM"), 1, GL_FALSE,
                           glm::value_ptr(mat_m));
    }

    void use(tue::gfx::vertex_array vao) {
        if (vao.id != m_vao.id) {
            glBindVertexArray(vao.id);
            m_vao = vao;
        }
    }

    void draw(tue::gfx::vertex_buffer vbo, GLenum mode) {
        glDrawArrays(mode, 0, vbo.count);
    }
};

struct default_camera {
    glm::vec3 pos{0, 0, -1};
    glm::vec3 at{0};
    glm::vec3 up{0, 1, 0};

    float fov{60.};
    float near{0.1};
    float far{1000};
};

} // namespace tue::gfx
