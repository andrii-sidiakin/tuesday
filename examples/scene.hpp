#pragma once

#include "render_context.hpp"

#include <type_traits>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct drawable {
    virtual ~drawable() = default;
    virtual void init(tue::gfx::render_context &) = 0;
    virtual void draw(tue::gfx::render_context &) = 0;
    virtual void update([[maybe_unused]] float dt) {}

    friend void tue_init(tue::gfx::render_context &c, drawable &o) {
        o.init(c);
    }
    friend void tue_draw(tue::gfx::render_context &c, drawable &o) {
        o.draw(c);
    }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct sync_context {
    delta_time dt{};
};

struct syncable {
    virtual ~syncable() = default;
    virtual void init(sync_context &) = 0;
    virtual void sync(sync_context &) = 0;

    friend void tue_init(sync_context &c, syncable &o) { o.init(c); }
    friend void tue_eval(sync_context &c, syncable &o) { o.sync(c); }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct collidable {
  protected:
    virtual ~collidable() = default;

  public:
    virtual void collide() = 0;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct drawable_noop : drawable {
    void init(tue::gfx::render_context & /*unused*/) noexcept final {}
    void draw(tue::gfx::render_context & /*unused*/) noexcept final {}
};

struct syncable_noop : syncable {
    void init(sync_context & /*unused*/) noexcept final {}
    void sync(sync_context & /*unused*/) noexcept final {}
};

struct collidable_noop : collidable {
    void collide() noexcept final {};
};

template <typename T> struct is_drawable : std::false_type {};
template <typename T> struct is_syncable : std::false_type {};
template <typename T> struct is_collidable : std::false_type {};

template <typename T> constexpr auto is_drawable_v = is_drawable<T>::value;
template <typename T> constexpr auto is_syncable_v = is_syncable<T>::value;
template <typename T> constexpr auto is_collidable_v = is_collidable<T>::value;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class scene_object {
  private:
    struct model_base : drawable, syncable {
        virtual ~model_base() = default;
        using drawable::init;
        using syncable::init;
    };

    template <typename T, typename = void> struct model_impl;

    template <typename T> struct model_impl<T> final : model_base {
        T data;

        model_impl(T &&d) : data(std::move(d)) {}
        model_impl(const T &d) : data(d) {}

        model_impl(model_impl &&) noexcept = default;
        model_impl &operator=(model_impl &&) noexcept = default;

        void init(tue::gfx::render_context &ctx) override {
            tue::gfx::init(ctx, data);
        }
        void draw(tue::gfx::render_context &ctx) override {
            tue::gfx::draw(ctx, data);
        }

        void init(sync_context &ctx) override { tue::gfx::init(ctx, data); }
        void sync(sync_context &ctx) override { tue::gfx::sync(ctx, data); }
    };

    template <typename T> struct model_impl<T &> final : model_base {
        T &data_ref;

        model_impl(T &ref) : data_ref(ref) {}

        model_impl(model_impl &&) noexcept = default;
        model_impl &operator=(model_impl &&) noexcept = default;

        void init(tue::gfx::render_context &ctx) override {
            tue::gfx::init(ctx, data_ref);
        }
        void draw(tue::gfx::render_context &ctx) override {
            tue::gfx::draw(ctx, data_ref);
        }

        void init(sync_context &ctx) override { tue::gfx::init(ctx, data_ref); }
        void sync(sync_context &ctx) override { tue::gfx::sync(ctx, data_ref); }
    };

  public:
    template <typename T>
    explicit scene_object(T &&obj)
        : m_model{std::make_unique<model_impl<T>>(std::forward<T>(obj))} {}

    void init(tue::gfx::render_context &ctx) {
        if (m_model) {
            m_model->init(ctx);
        }
    }
    void draw(tue::gfx::render_context &ctx) {
        if (m_model) {
            m_model->draw(ctx);
        }
    }

    void init(sync_context &ctx) {
        if (m_model) {
            m_model->init(ctx);
        }
    }
    void sync(sync_context &ctx) {
        if (m_model) {
            m_model->sync(ctx);
        }
    }

  private:
    std::unique_ptr<model_base> m_model;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class scene_root {
  public:
    scene_root() = default;

  public:
    void resize(int width, int height) {
        m_draw_ctx.set_viewport(width, height);
    }

    void init() {
        m_draw_ctx.mat_p = m_draw_ctx.mat_v = m_draw_ctx.mat_m = glm::mat4{1};
        m_sync_ctx.dt = delta_time::zero();

        for (auto &o : m_objs) {
            o.init(m_sync_ctx);
            o.init(m_draw_ctx);
        }
    }

    void draw() {
        for (auto &o : m_objs) {
            o.draw(m_draw_ctx);
        }
    }

    void sync(delta_time dt) {
        m_sync_ctx.dt = dt;
        for (auto &o : m_objs) {
            o.sync(m_sync_ctx);
        }
    }

  public:
    template <typename T> void emplace_back(T &&obj) {
        m_objs.emplace_back(std::forward<T>(obj));
    }

  private:
    tue::gfx::render_context m_draw_ctx;
    sync_context m_sync_ctx;
    std::vector<scene_object> m_objs;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct clear_scene {
    GLfloat clear_color[4] = {0, 0, 0.1, 1};
    GLbitfield clear_bits = {GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT};

    friend constexpr void
    tue_init([[maybe_unused]] tue::gfx::render_context &ctx,
             clear_scene o) noexcept {
        glClearColor(o.clear_color[0], o.clear_color[1], o.clear_color[2],
                     o.clear_color[3]);
    }

    friend constexpr void
    tue_draw([[maybe_unused]] tue::gfx::render_context &ctx,
             clear_scene o) noexcept {
        glClear(o.clear_bits);
    }

    friend constexpr void tue_init([[maybe_unused]] sync_context &ctx,
                                   [[maybe_unused]] clear_scene o) noexcept {}

    friend constexpr void tue_sync([[maybe_unused]] sync_context &ctx,
                                   [[maybe_unused]] clear_scene o) noexcept {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct camera {
    glm::vec3 pos{0, 0, -1};
    glm::vec3 at{0};
    glm::vec3 up{0, 1, 0};

    float fov{60.};
    float near{0.1};
    float far{1000};

    friend constexpr void
    tue_init([[maybe_unused]] tue::gfx::render_context &ctx,
             camera &o) noexcept {
        o.pos = glm::vec3{0, 3, -10};
        o.at = glm::vec3{0, 0, 1};
        o.up = glm::vec3{0, 1, 0};

        o.near = 0.1;
        o.far = 1000;
    }

    friend constexpr void tue_draw(tue::gfx::render_context &ctx,
                                   camera &o) noexcept {
        if (!tue_assert(ctx.aspect_ratio > 0)) {
            return;
        }

        float ar = ctx.width / ctx.height;
        ctx.mat_p = glm::perspective(glm::radians(o.fov), ar, o.near, o.far);
        ctx.mat_v = glm::lookAt(o.pos, o.at, o.up);
    }

    friend constexpr void tue_init([[maybe_unused]] sync_context &ctx,
                                   [[maybe_unused]] camera &o) noexcept {}

    friend constexpr void tue_sync([[maybe_unused]] sync_context &ctx,
                                   [[maybe_unused]] camera &o) noexcept {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
