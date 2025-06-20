#include <tuesday/gfx.hpp>
#include <tuesday/wsi.hpp>

#include "helpers.hpp"
#include "scene.hpp"

#include <print>

struct particle_system {

    struct particle {
        glm::vec3 v;
        glm::vec3 f;
    };

    tue::gfx::vertex_array vao{};
    tue::gfx::vertex_buffer vbo_one{};
    tue::gfx::shader_program shader{};

    struct attr_data {
        tue::gfx::vertex_attrib_format attr{};
        tue::gfx::vertex_buffer vbo{};
        std::size_t size{0};
        const void *data{nullptr};
    };

    struct model_data {
        std::vector<particle> parts;
        std::vector<glm::vec3> pos;
        std::vector<glm::u8vec3> clr;

        attr_data attrs[2];

        void reset(std::size_t count) {
            parts.resize(count);
            pos.resize(count);
            clr.resize(count);

            attrs[0] = attr_data{
                tue::gfx::vertex_attrib_format_for<glm::vec3>,
                tue::gfx::create_vertex_buffer_for<glm::vec3>(count,
                                                              GL_STREAM_DRAW),
                count * sizeof(glm::vec3),
                pos.data(),
            };
            attrs[1] = attr_data{
                tue::gfx::vertex_attrib_format_for<glm::u8vec3>,
                tue::gfx::create_vertex_buffer_for<glm::u8vec3>(count,
                                                                GL_STREAM_DRAW),
                count * sizeof(glm::u8vec3),
                clr.data(),
            };
        }
    };

    static constexpr auto init_radius = 6.F;
    static constexpr auto part_radius = 0.02F;
    static constexpr auto part_mesh = cube_mesh(part_radius);

    std::size_t part_count{0};
    model_data data;

    explicit particle_system(std::size_t count) : part_count{count} {}

    particle_system(particle_system &&) noexcept = default;
    particle_system &operator=(particle_system &&) noexcept = default;

    ~particle_system() {
        for (auto &a : data.attrs) {
            delete_vertex_buffer(a.vbo);
        }
        delete_vertex_buffer(vbo_one);
        delete_vertex_array(vao);
        delete_shader(shader);
    }

    void init([[maybe_unused]] tue::gfx::render_context &ctx) {
        data.reset(part_count);

        for (size_t i{0}; i < part_count; ++i) {
            data.pos[i] =
                glm::vec3{0, init_radius, 0} + glm::ballRand(init_radius);
            data.clr[i] = glm::vec3{255.F} * glm::ballRand(1.F);
        }

        const GLuint one_binding_index = 0;
        vao = tue::gfx::create_vertex_array();

        vbo_one = tue::gfx::create_vertex_buffer(part_mesh, GL_STATIC_DRAW);
        bind_buffer(vao, one_binding_index, vbo_one);
        auto one_fmt = tue::gfx::vertex_attrib_format_for<glm::vec3>;
        one_fmt.index = one_binding_index;
        bind_attrib(vao, one_binding_index, one_fmt);

        glVertexArrayBindingDivisor(vao.id, one_binding_index, 0);

        auto abi = one_binding_index + 1;
        for (auto &a : data.attrs) {
            bind_buffer(vao, abi, a.vbo);
            a.attr.index = abi;
            bind_attrib(vao, abi, a.attr);
            glVertexArrayBindingDivisor(vao.id, abi, 1);
            abi++;
        }

        shader = tue::gfx::make_shader(vert_source_inst, frag_source);
    }

    void draw(tue::gfx::render_context &ctx) {
        ctx.use(shader);
        ctx.use(vao);

        for (auto &a : data.attrs) {
            glNamedBufferData(a.vbo.id, a.size, nullptr, GL_STREAM_DRAW);
            glNamedBufferSubData(a.vbo.id, 0, a.size, a.data);
        }

        glDrawArraysInstanced(GL_TRIANGLES, 0, part_mesh.size(), part_count);
    }

    void sync(sync_context &ctx) {
        const glm::vec3 G = {0, -9.8, 0};

        auto dt = ctx.dt.count();

        for (size_t i{0}; i < data.parts.size(); ++i) {
            auto mi = 1.f; // mass inverted
            auto a = data.parts[i].f + G * mi;
            auto v = data.parts[i].v + a * dt;
            auto x = data.pos[i] + v * dt;

            if (x.y <= 0) {
                x.y = 0;
                v.y = -v.y * 0.8;
            }

            data.pos[i] = x;
            data.parts[i].v = v;
            data.parts[i].f = glm::vec3{0};
        }
    }

    friend void tue_init(tue::gfx::render_context &ctx, particle_system &o) {
        o.init(ctx);
    }

    friend void tue_draw(tue::gfx::render_context &ctx, particle_system &o) {
        o.draw(ctx);
    }

    friend void tue_init([[maybe_unused]] sync_context &ctx,
                         [[maybe_unused]] particle_system &o) {}

    friend void tue_sync(sync_context &ctx, particle_system &o) { o.sync(ctx); }
};

static void gl_debug(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message, const void *param);

int main() {
    constexpr std::size_t NumParts = 10'000;

    // setup scene
    scene_root scn;
    scn.emplace_back(clear_scene{});
    scn.emplace_back(camera{});
    scn.emplace_back(particle_system(NumParts));

    auto wsi = tue::wsi::connect();
    auto wnd = wsi.make_window();
    auto *wcb = wnd.make_watcher();

    // setup callbacks

    wcb->on_size = [&scn](auto & /*wnd*/, auto e) {
        scn.resize(e.size.width, e.size.height);
    };

    wcb->on_keyboard = [&scn](auto &w, auto e) {
        using tue::wsi::key_code;
        if (e.pressed(key_code::escape)) {
            w.close();
        }
        else if (e.pressed(key_code::F)) {
            w.toggle_fullscreen();
        }
        else if (e.pressed(key_code::space)) {
            scn.init();
            scn.resize(w.get_attrs().size.width, w.get_attrs().size.height);
        }
    };

    wcb->on_draw = [&scn](auto & /*wnd*/) {
        std::println("draw requested");
        scn.draw();
    };

    wcb->on_mouse = [](auto & /*wnd*/, auto e) {
        using tue::wsi::key_code;
        using tue::wsi::key_mods;
        if (e.holds(key_mods::alt) && e.pressed(key_code::mouse_left)) {
            std::println("{}x{}", e.pos.x, e.pos.y);
        }
    };

    auto attrs = wnd.get_attrs()
                     .with(tue::wsi::window_size{800, 600})
                     .with(tue::wsi::window_mode::normal);

    // open the window
    wnd.set_attrs(attrs);
    wnd.open();
    wsi.make_current(wnd);

    // load/setup GL
    auto glx = wsi.make_gl_context();
    int glver = gladLoadGL(glx.proc_addr);
    std::println("GL v.{}", glver);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_debug, nullptr);

    scn.init();
    scn.resize(wnd.get_attrs().size.width, wnd.get_attrs().size.height);

    time_point t0 = clock_type::now();
    time_point t_iter = t0;

    delta_time stat_rate = std::chrono::seconds{1};

    time_point t_sim = t0;
    delta_time sim_rate = std::chrono::milliseconds{1};
    std::size_t num_sims = 0;

    time_point t_draw = t0;
    delta_time draw_rate = std::chrono::milliseconds(10);
    std::size_t num_draws = 0;

    while (!wnd.should_close()) {
        wsi.poll_events();

        auto t = clock_type::now();
        auto dt = delta_time{t - t_iter};
        t_iter = t;

        auto dt_sim = t - t_sim;
        if (dt_sim >= sim_rate) {
            auto sim_dt_acc = delta_time::zero();
            while (sim_dt_acc < dt_sim) {
                scn.sync(sim_rate);
                sim_dt_acc += sim_rate;
                num_sims++;
            }
            t_sim +=
                std::chrono::duration_cast<time_point::duration>(sim_dt_acc);
        }

        if ((t - t_draw) >= draw_rate) {
            t_draw = t;
            scn.draw();
            wnd.swap_buffers();
            num_draws += 1;
        }

        stat_rate -= dt;
        if (stat_rate.count() <= 0) {
            std::println("fps={}; ips={}", num_draws, num_sims);
            stat_rate = std::chrono::seconds{1};
            num_draws = 0;
            num_sims = 0;
        }
    }

    // force GL objects cleanup before context (window) get destroyed
    scn = {};

    return 0;
}

static void gl_debug([[maybe_unused]] GLenum source,
                     [[maybe_unused]] GLenum type, [[maybe_unused]] GLuint id,
                     [[maybe_unused]] GLenum severity, GLsizei length,
                     const GLchar *message,
                     [[maybe_unused]] const void *param) {
    std::println("[gl] {}", std::string_view(message, message + length));
}
