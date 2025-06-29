#include <base_scene.hpp>
#include <demo_app.hpp>

#include "particles_system.hpp"

class demo1_scene : public base_scene {
  public:
    static constexpr auto init_radius = 6.F;
    static constexpr auto part_radius = 0.02F;
    static constexpr auto part_mesh = cube_mesh(part_radius);

    struct model_data {
        struct attr_data {
            tue::gfx::vertex_attrib_format attr{};
            tue::gfx::vertex_buffer vbo{};
            std::size_t size{0};
            const void *data{nullptr};
        };

        attr_data attrs[2];

        void reset(EntityRegistry &reg) {
            const auto &pos = reg.use_component<Position>();
            const auto &clr = reg.use_component<Color>();

            attrs[0] = attr_data{
                tue::gfx::vertex_attrib_format_for<glm::vec3>,
                tue::gfx::create_vertex_buffer_for<glm::vec3>(pos.size(),
                                                              GL_STREAM_DRAW),
                pos.size() * sizeof(glm::vec3),
                pos.data(),
            };
            attrs[1] = attr_data{
                tue::gfx::vertex_attrib_format_for<glm::u8vec3>,
                tue::gfx::create_vertex_buffer_for<glm::u8vec3>(clr.size(),
                                                                GL_STREAM_DRAW),
                clr.size() * sizeof(glm::u8vec3),
                clr.data(),
            };
        }
    };

    std::size_t part_count{0};
    tue::gfx::vertex_array vao{};
    tue::gfx::vertex_buffer vbo_one{};
    tue::gfx::shader_program shader{};
    model_data m_data;

  public:
    void cleaup() {
        for (auto &a : m_data.attrs) {
            delete_vertex_buffer(a.vbo);
        }
        delete_vertex_buffer(vbo_one);
        delete_vertex_array(vao);
        delete_shader(shader);
    }

    void reset() override {

        part_count = 10'000U;
        m_reg = {};

        m_reg.make_system<PhysicsSystem>(m_reg);
        m_reg.make_system<CollisionSystem>(m_reg);

        for (size_t i{0}; i < part_count; ++i) {
            m_reg.insert(Entity::make(),
                         Position{glm::vec3{0, init_radius, 0} +
                                  glm::ballRand(init_radius)},
                         Color{glm::vec3{255.F} * glm::ballRand(1.F)}, Force{},
                         Velocity{});
        }

        m_data.reset(m_reg);

        const GLuint one_binding_index = 0;
        vao = tue::gfx::create_vertex_array();

        vbo_one = tue::gfx::create_vertex_buffer(part_mesh, GL_STATIC_DRAW);
        bind_buffer(vao, one_binding_index, vbo_one);
        auto one_fmt = tue::gfx::vertex_attrib_format_for<glm::vec3>;
        one_fmt.index = one_binding_index;
        bind_attrib(vao, one_binding_index, one_fmt);

        glVertexArrayBindingDivisor(vao.id, one_binding_index, 0);

        auto abi = one_binding_index + 1;
        for (auto &a : m_data.attrs) {
            bind_buffer(vao, abi, a.vbo);
            a.attr.index = abi;
            bind_attrib(vao, abi, a.attr);
            glVertexArrayBindingDivisor(vao.id, abi, 1);
            abi++;
        }

        shader = tue::gfx::make_shader(vert_source_inst, frag_source);
    }

    void update(delta_time dt_) override {
        m_reg.use_system<PhysicsSystem>().update(dt_.count());
        m_reg.use_system<CollisionSystem>().update(dt_.count());
    }

    void render(render_context &ctx) override {
        ctx.use(shader);
        ctx.use(vao);

        for (auto &a : m_data.attrs) {
            glNamedBufferData(a.vbo.id, a.size, nullptr, GL_STREAM_DRAW);
            glNamedBufferSubData(a.vbo.id, 0, a.size, a.data);
        }

        glDrawArraysInstanced(GL_TRIANGLES, 0, part_mesh.size(), part_count);
    }

  private:
    EntityRegistry m_reg;
};

int main() {
    demo_app app;
    demo1_scene scene;
    app.set_scene(scene);
    app.run();
    return 0;
}
