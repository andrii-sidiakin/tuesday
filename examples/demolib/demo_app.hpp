#include "base_app.hpp"
#include "base_scene.hpp"

#include "glad/gl.h"
#include "render_context.hpp"

class demo_app : public base_app {
  public:
    static constexpr GLfloat bg[] = {0.1, 0.1, 0.1, 1.0};
    static constexpr GLbitfield clear_bits =
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    static constexpr tue::wsi::window_size window_size{800, 600};

    demo_app() { wnd_attrs = {.size = window_size}; }

    void set_scene(base_scene &scene) { m_scene = &scene; }

  private:
    bool done() const noexcept override { return m_scene == nullptr; }

    void reset() override {
        m_scene->reset();

        m_camera.pos = glm::vec3{0, 3, -10};
        m_camera.at = glm::vec3{0, 0, 1};
        m_camera.up = glm::vec3{0, 1, 0};
        m_camera.fov = 60.;
        m_camera.near = 0.1;
        m_camera.far = 1000;
    }

    void step(delta_time dt) override { m_scene->update(dt); }

    void draw() override {
        glClearColor(bg[0], bg[1], bg[2], bg[3]);
        glClear(clear_bits);
        m_scene->render(m_context);
    }

    void resize(tue::wsi::window & /*w*/, tue::wsi::resize_event e) override {
        m_context.set_viewport(e.size.width, e.size.width);

        float ar = m_context.width / m_context.height;
        m_context.mat_p = glm::perspective(glm::radians(m_camera.fov), ar,
                                           m_camera.near, m_camera.far);
        m_context.mat_v = glm::lookAt(m_camera.pos, m_camera.at, m_camera.up);
    }

  private:
    base_scene *m_scene{nullptr};
    render_context m_context;
    camera m_camera{};
};
