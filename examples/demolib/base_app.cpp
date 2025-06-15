#include "base_app.hpp"
#include "helpers.hpp"

#include <print>

bool base_app::done() const {
    return false;
}

void base_app::resize([[maybe_unused]] tue::wsi::window &w,
                      [[maybe_unused]] tue::wsi::resize_event e) {
}

void base_app::redraw([[maybe_unused]] tue::wsi::window &w) {
}

void base_app::keyboard(tue::wsi::window &w, tue::wsi::keyboard_event e) {
    using tue::wsi::key_code;
    if (e.pressed(key_code::escape)) {
        w.close();
    }
    else if (e.pressed(key_code::F)) {
        w.toggle_fullscreen();
    }
    else if (e.pressed(key_code::space)) {
        reset();
    }
    else {
        std::println("keyboard: code={}", (int)e.code);
    }
}

void base_app::mouse([[maybe_unused]] tue::wsi::window &w,
                     [[maybe_unused]] tue::wsi::mouse_event e) {
    if (e.act == tue::wsi::key_action::pressed) {
        std::println("mouse: code={}", (int)e.code);
    }
}

void base_app::run() {
    m_wsi = tue::wsi::connect();
    m_wnd = m_wsi.make_window(wnd_attrs);
    watch(m_wnd);

    m_wnd.open();
    m_wsi.make_current(m_wnd);

    auto glx = m_wsi.make_gl_context();
    int glver = gladLoadGL(glx.proc_addr);
    if (glver == 0) {
        std::println("Failed to load OpenGL");
        return;
    }

    std::println("GL v.{}", glver);

    do_reset();

    time_point t0 = clock_type::now();
    time_point ti = t0;
    delta_time dt_stat = delta_time::zero();

    while (!done() && !m_wnd.should_close()) {
        auto t = clock_type::now();
        auto dt = delta_time{t - ti};
        ti = t;

        do_step();

        if (do_draw()) {
            m_wsi.poll_events();
            m_wnd.swap_buffers();
        }

        dt_stat += dt;
        if (dt_stat > delta_time{1}) {
            dt_stat -= delta_time{1};
            std::println("step:{:.2f}; draw:{:.2f}", stat.step_fps(),
                         stat.draw_fps());
        }
    }
}

void base_app::do_reset() {
    stat = {};
    m_t_init = clock_type::now();
    m_t_step = m_t_init;
    m_t_draw = m_t_init;

    reset();
}

bool base_app::do_step() {
    auto t = clock_type::now();
    auto dt = delta_time{t - m_t_step};
    step(dt);
    auto t_frame = clock_type::now();
    stat.step.add_frame(delta_time{t_frame - t});
    m_t_step = t;
    return true;
}

bool base_app::do_draw() {
    auto t = clock_type::now();
    auto dt = delta_time{t - m_t_draw};

    if (dt < draw_rate) {
        return false;
    }

    draw();
    auto t_frame = clock_type::now();

    m_t_draw = t;
    stat.draw.add_frame(delta_time{t_frame - t});
    return true;
}

void base_app::watch(tue::wsi::window &wnd) {
    auto *wcb = wnd.make_watcher();
    tue_assert(wcb != nullptr);
    wcb->on_size = [this](auto &w, auto e) { resize(w, e); };
    wcb->on_draw = [this](auto &w) { redraw(w); };
    wcb->on_keyboard = [this](auto &w, auto e) { keyboard(w, e); };
    wcb->on_mouse = [this](auto &w, auto e) { mouse(w, e); };
}
