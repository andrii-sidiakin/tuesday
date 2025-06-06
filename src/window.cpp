#include <tuesday/wsi/window.hpp>

#include "glfw.hpp"

namespace tue::wsi {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

window_watcher::window_watcher(window &owner) : m_owner{&owner} {
}

window_watcher::~window_watcher() = default;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void window::open() {
    if (m_client) {
        m_client->open();
    }
}

void window::open(window_size size) {
    m_attrs.size = size;
    open();
}

void window::close() {
    if (m_client) {
        m_client->close();
    }
}

bool window::should_close() const {
    return m_client == nullptr || m_client->should_close();
}

void window::swap_buffers() {
    if (m_client) {
        m_client->redraw();
    }
}

void window::toggle_fullscreen() {
    if (m_client) {
        if (m_attrs.mode == window_mode::normal) {
            m_attrs.mode = window_mode::fullscreen;
            m_client->reload_attrs();
        }
        else if (m_attrs.mode == window_mode::fullscreen) {
            m_attrs.mode = window_mode::normal;
            m_client->reload_attrs();
        }
    }
}

void window::set_attrs(window_attrs attrs) {
    m_attrs = attrs;
    if (m_client) {
        m_client->reload_attrs();
    }
}

window_watcher *window::make_watcher() {
    m_watchers.emplace_back(new window_watcher(*this));
    return m_watchers.back().get();
}

void window::remove_watcher(window_watcher *watcher) {
    auto iter = std::ranges::find_if(
        m_watchers, [w = watcher](auto &wp) { return wp.get() == w; });
    if (iter != end(m_watchers)) {
        m_watchers.erase(iter);
    }
}

void window::set_client(std::unique_ptr<window_client> client) {
    m_client = std::move(client);
    if (m_client) {
        m_client->set_context(this);
    }
}

window_client *window::get_client_ptr() const noexcept {
    return m_client.get();
}

window_pos window::get_normal_pos() const noexcept {
    return m_attrs.pos;
}

window_size window::get_normal_size() const noexcept {
    return m_attrs.size;
}

window_mode window::get_default_mode() const noexcept {
    return m_attrs.mode;
}

void window::on_event(resize_event e) noexcept {
    for (auto &w : m_watchers) {
        if (w->on_size) {
            w->on_size(*this, e);
        }
    }
}

void window::on_event(redraw_event e) noexcept {
    for (auto &w : m_watchers) {
        if (w->on_draw) {
            w->on_draw(*this);
        }
    }
}

void window::on_event(keyboard_event e) noexcept {
    for (auto &w : m_watchers) {
        if (w->on_keyboard) {
            w->on_keyboard(*this, e);
        }
    }
}

void window::on_event(mouse_event e) noexcept {
    for (auto &w : m_watchers) {
        if (w->on_mouse) {
            w->on_mouse(*this, e);
        }
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

window window_system::make_window(window_attrs attrs) {
    window wnd;
    if (m_client) {
        wnd.set_attrs(attrs);
        wnd.set_client(m_client->make_window_client());
    }
    return wnd;
}

void window_system::poll_events() {
    if (m_client) {
        m_client->poll_events();
    }
}

gl_context window_system::make_gl_context() const noexcept {
    if (m_client) {
        return m_client->make_gl_context();
    }
    return {};
}

void window_system::make_current(window &wnd) {
    if (auto *wc = wnd.get_client_ptr()) {
        m_client->make_current(*wc);
    }
}

void window_system::set_client(std::unique_ptr<window_system_client> client) {
    m_client = std::move(client);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

window_system connect() {
    window_system ws;
    ws.set_client(glfw::make_window_system_client());
    return ws;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

key_code key_code_from_ascii(char c) noexcept {

    if (c >= '0' && c <= '9') {
        return static_cast<key_code>(c);
    }
    if (c >= 'A' && c <= 'Z') {
        return static_cast<key_code>(c);
    }
    if (c >= 'a' && c <= 'z') {
        return static_cast<key_code>(c - ('a' - 'A'));
    }

    return key_code::unknown;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::wsi
