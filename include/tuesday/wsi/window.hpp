#ifndef _TUE_WSI_WINDOW_HPP_INCLUDED_
#define _TUE_WSI_WINDOW_HPP_INCLUDED_

#include "tuesday/wsi/window_attrs.hpp"
#include <tuesday/wsi/window_client.hpp>

#include <functional>
#include <list>

namespace tue::wsi {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class window;
class window_system;

class window_watcher {
  private:
    friend window;
    explicit window_watcher(window &);

  public:
    ~window_watcher();

  public:
    template <typename... Args> using signal = std::function<void(Args...)>;

    signal<window &, resize_event> on_size;
    signal<window &> on_draw;
    signal<window &, keyboard_event> on_keyboard;
    signal<window &, mouse_event> on_mouse;

  private:
    window *m_owner{nullptr};
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class window : public window_context {
  public:
    window() = default;
    window(window &&other) noexcept;
    window &operator=(window &&other) noexcept;
    void swap(window &other) noexcept;

  public:
    void open();
    void open(window_size size);
    void close();

    bool should_close() const;

    void swap_buffers();
    void toggle_fullscreen();

  public:
    void set_attrs(window_attrs attrs);
    window_attrs get_attrs() const noexcept { return m_attrs; }

  public:
    window_watcher *make_watcher();
    void remove_watcher(window_watcher *w);

  public:
    void set_client(std::unique_ptr<window_client>);

  protected:
    friend window_system;
    window_client *get_client_ptr() const noexcept;

  protected:
    window_pos get_normal_pos() const noexcept override;
    window_size get_normal_size() const noexcept override;
    window_mode get_default_mode() const noexcept override;
    void on_event(resize_event e) noexcept override;
    void on_event(redraw_event e) noexcept override;
    void on_event(keyboard_event e) noexcept override;
    void on_event(mouse_event e) noexcept override;

  private:
    std::unique_ptr<window_client> m_client;
    std::list<std::unique_ptr<window_watcher>> m_watchers;
    window_attrs m_attrs{};
    window_size m_normal_size{};
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class window_system {
  public:
    window make_window(window_attrs attrs = {});

    void poll_events();

    gl_context make_gl_context() const noexcept;
    void make_current(window &wnd);

  public:
    void set_client(std::unique_ptr<window_system_client>);

  private:
    std::unique_ptr<window_system_client> m_client;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::wsi

#endif
