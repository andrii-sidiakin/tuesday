#include "base_app.hpp"
#include "base_scene.hpp"

class demo_app : public base_app {
  public:
    static constexpr GLfloat bg[] = {0.1, 0.1, 0.1, 1.0};
    static constexpr GLbitfield clear_bits = GL_COLOR_BUFFER_BIT;
    static constexpr tue::wsi::window_size window_size{800, 600};

    demo_app() { wnd_attrs = {.size = window_size}; }

    void set_scene(base_scene &scene) { m_scene = &scene; }

  private:
    bool done() const noexcept override { return m_scene == nullptr; }

    void reset() override { m_scene->reset(); }

    void step(delta_time dt) override { m_scene->update(dt); }

    void draw() override {
        glClearColor(bg[0], bg[1], bg[2], bg[3]);
        glClear(clear_bits);
    }

  private:
    base_scene *m_scene{nullptr};
};
