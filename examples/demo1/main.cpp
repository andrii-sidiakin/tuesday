#include <base_scene.hpp>
#include <demo_app.hpp>

class demo1_scene : public base_scene {
  public:
    void reset() override {}
    void update(delta_time dt) override {}
    void render(render_context &ctx) override {}
};

int main() {
    demo_app app;
    demo1_scene scene;
    app.set_scene(scene);
    app.run();
    return 0;
}
