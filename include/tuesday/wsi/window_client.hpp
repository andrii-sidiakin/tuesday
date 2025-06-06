#ifndef _TUE_WSI_WINDOW_CLIENT_HPP_INCLUDED_
#define _TUE_WSI_WINDOW_CLIENT_HPP_INCLUDED_

#include <tuesday/wsi/window_attrs.hpp>
#include <tuesday/wsi/window_events.hpp>

#include <memory>

namespace tue::wsi {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class window_context {
  public:
    virtual ~window_context() = default;

    virtual window_pos get_normal_pos() const noexcept = 0;
    virtual window_size get_normal_size() const noexcept = 0;
    virtual window_mode get_default_mode() const noexcept = 0;

    virtual void on_event(resize_event) = 0;
    virtual void on_event(redraw_event) = 0;
    virtual void on_event(keyboard_event) = 0;
    virtual void on_event(mouse_event) = 0;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class window_client {
  public:
    window_client() = default;
    virtual ~window_client() = default;

    window_context *context() const noexcept { return m_context; }
    void set_context(window_context *ctx) noexcept { m_context = ctx; }

    virtual void open() = 0;
    virtual void close() = 0;
    virtual void reload_attrs() = 0;
    virtual void redraw() = 0;

    virtual bool should_close() const = 0;

  private:
    window_context *m_context{nullptr};
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct gl_context {
    using proc_fn = void (*)();
    using proc_addr_fn = proc_fn (*)(const char *);

    proc_addr_fn proc_addr{};
};

class window_system_client {
  public:
    virtual ~window_system_client() = default;

    virtual std::unique_ptr<window_client> make_window_client() = 0;

    virtual void poll_events() = 0;

    virtual gl_context make_gl_context() const noexcept = 0;
    virtual void make_current(window_client &wc) noexcept = 0;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::wsi

#endif
