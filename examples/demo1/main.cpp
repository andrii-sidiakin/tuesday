#include <base_scene.hpp>
#include <demo_app.hpp>

#include "particle.hpp"
#include "render.hpp"

template <std::size_t NumAttrs> struct instance_buffer {
    struct attr_data {
        tue::gfx::vertex_attrib_format attr{};
        tue::gfx::vertex_buffer vbo{};
        std::size_t size{0};
        const void *data{nullptr};
    };

    void release() {
        for (auto &a : attrs) {
            tue::gfx::delete_vertex_buffer(a.vbo);
        }
        tue::gfx::delete_vertex_buffer(vbo);
        tue::gfx::delete_vertex_array(vao);
    }

    template <class T>
    void alloc_attr(std::size_t i, std::size_t count, const T *data = nullptr) {
        tue_assert(i < NumAttrs, "Out of range");
        tue_assert(!attrs[i].vbo, "Already in use");
        tue_assert(count > 0, "Expect count > 0");
        attrs[i] = attr_data{
            tue::gfx::vertex_attrib_format_for<T>,
            tue::gfx::create_vertex_buffer_for<T>(count, GL_STREAM_DRAW),
            count * sizeof(T),
            data,
        };
    }

    tue::gfx::vertex_array vao{};
    tue::gfx::vertex_buffer vbo{};
    attr_data attrs[NumAttrs]{};
};

class demo1_scene : public base_scene {
  public:
    static constexpr auto num_parts = 1'000U;
    static constexpr auto part_size = 0.01F;

    void reset() override {
        m_parts_obj.release();

        m_parts.resize(num_parts);

        m_parts_obj.vao = tue::gfx::create_vertex_array();

        auto cube = cube_mesh(part_size);
        m_parts_obj.vbo = tue::gfx::create_vertex_buffer(cube, GL_STATIC_DRAW);
        tue::gfx::bind_buffer(m_parts_obj.vao, 0, m_parts_obj.vbo);
        tue::gfx::bind_attrib(m_parts_obj.vao, 0,
                              tue::gfx::vertex_attrib_format_for<glm::vec3>);

        m_parts_obj.alloc_attr(0, num_parts, m_parts.data.x.data());
        m_parts_obj.alloc_attr(1, num_parts, m_parts.data.v.data());
    }

    void update(delta_time dt) override {
        const glm::vec3 fG{0, -9.8, 0};
        m_parts.apply_force(fG);
        m_parts.step(dt);
    }

    void render(render_context &ctx) override {
        //
    }

  private:
    particle_system m_parts;
    instance_buffer<2> m_parts_obj;
};

int main() {
    demo_app app;
    demo1_scene scene;
    app.set_scene(scene);
    app.run();
    return 0;
}
