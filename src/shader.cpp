#include <tuesday/gfx/shader.hpp>

namespace tue::gfx {

shader_stage make_shader_stage(std::string_view src, GLenum type) noexcept {
    GLuint id = glCreateShader(type);

    const GLchar *ptr = (GLchar *)src.data();
    glShaderSource(id, 1, &ptr, nullptr);
    glCompileShader(id);

    GLint status = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        return {};
    }

    return shader_stage{id, type};
}

void delete_shader_stage(shader_stage &s) noexcept {
    glDeleteShader(s.id);
    s = {};
}

shader_program link_shader(std::span<shader_stage> stages) noexcept {
    GLint status{0};

    GLuint id = glCreateProgram();
    for (auto &stage : stages) {
        glAttachShader(id, stage.id);
    }
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == 0) {
        return {};
    }

    return shader_program{.id = id};
}

shader_program make_shader(std::string_view vs_src,
                           std::string_view fs_src) noexcept {
    shader_stage stages[] = {
        make_shader_stage(vs_src, GL_VERTEX_SHADER),
        make_shader_stage(fs_src, GL_FRAGMENT_SHADER),
    };
    auto sp = link_shader(std::span{stages, 2});
    for (auto &ss : stages) {
        delete_shader_stage(ss);
    }
    return sp;
}

void delete_shader(shader_program &s) noexcept {
    if (s) {
        glDeleteProgram(s.id);
    }
    s = {};
}

GLint find_uniform(shader_program p, std::string_view name) noexcept {
    return glGetUniformLocation(p.id, name.data());
}

} // namespace tue::gfx
