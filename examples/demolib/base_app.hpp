#pragma once

#include <tuesday/assert.hpp>
#include <tuesday/wsi.hpp>

#include "helpers.hpp"

struct frame_stat_data {
    std::size_t num_frames{0};
    delta_time frame_total{0};

    constexpr void add_frame(delta_time dt) noexcept {
        frame_total += dt;
        num_frames += 1;
    }

    constexpr auto frame_avg() const noexcept {
        return delta_time{num_frames > 0 ? frame_total.count() / num_frames
                                         : 0.};
    }

    constexpr auto fps() const noexcept { return 1.f / frame_avg().count(); }
};

struct stat_data {
    frame_stat_data step;
    frame_stat_data draw;

    constexpr auto step_fps() const noexcept { return step.fps(); }
    constexpr auto draw_fps() const noexcept { return draw.fps(); }
};

class base_app {
  public:
    virtual ~base_app() = default;

    tue::wsi::window_attrs wnd_attrs{};

    delta_time step_rate{1. / 200};
    delta_time draw_rate{1. / 60};

    stat_data stat{};

    virtual bool done() const;
    virtual void reset() {}
    virtual void step(delta_time dt) {}
    virtual void draw() {}

    virtual void resize(tue::wsi::window &w, tue::wsi::resize_event e);
    virtual void redraw(tue::wsi::window &w);
    virtual void keyboard(tue::wsi::window &w, tue::wsi::keyboard_event e);
    virtual void mouse(tue::wsi::window &w, tue::wsi::mouse_event e);

  public:
    void run();

  private:
    void do_reset();
    bool do_step();
    bool do_draw();

    void watch(tue::wsi::window &wnd);

  private:
    tue::wsi::window_system m_wsi;
    tue::wsi::window m_wnd;

    time_point m_t_init;
    time_point m_t_step;
    time_point m_t_draw;
};
