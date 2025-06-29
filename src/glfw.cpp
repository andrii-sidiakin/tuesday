#include "glfw.hpp"

#include <map>
#include <print>

namespace glfw {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static tue::wsi::key_code key_code_from_glfw(int key) noexcept;
static tue::wsi::key_action key_action_from_glfw(int action) noexcept;
static tue::wsi::key_mods key_mods_from_glfw(int mods) noexcept;

static void assign_handle_client(GLFWwindow *h, window_client *c) noexcept;
static void erase_handle_client(GLFWwindow *h) noexcept;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

window_client::window_client(window_system_client &ws) noexcept
    : m_wndsys{&ws} {
}

window_client::window_client(window_client &&other) noexcept {
    other.swap(*this);
}

window_client &window_client::operator=(window_client &&other) noexcept {
    other.swap(*this);
    return *this;
}

void window_client::swap(window_client &other) noexcept {
    if (this != &other) {
        m_wndsys = std::exchange(other.m_wndsys, nullptr);
        m_handle = std::exchange(other.m_handle, nullptr);
        assign_handle_client(m_handle, this);
        assign_handle_client(other.m_handle, &other);
    }
}

window_client::~window_client() {
    if (m_wndsys != nullptr && m_handle != nullptr) {
        m_wndsys->destroy_handle(*this);
    }
}

void window_client::open() {
    if (m_wndsys != nullptr) {
        m_wndsys->create_handle(*this);
    }
}

void window_client::close() {
    if (m_handle != nullptr) {
        glfwSetWindowShouldClose(m_handle, GLFW_TRUE);
    }
}

void window_client::reload_attrs() {
    if (m_handle == nullptr) {
        return;
    }

    if (auto *ctx = context()) {
        auto mode = ctx->get_default_mode();
        GLFWmonitor *mon = glfwGetWindowMonitor(m_handle);
        if (mode == tue::wsi::window_mode::fullscreen) {
            if (mon == nullptr) {
                mon = glfwGetPrimaryMonitor();
                const GLFWvidmode *mode = glfwGetVideoMode(mon);
                glfwSetWindowMonitor(m_handle, mon, 0, 0, mode->width,
                                     mode->height, mode->refreshRate);
            } // else {} // already in a full screen mode
        }
        else {
            auto pos = ctx->get_normal_pos();
            auto size = ctx->get_normal_size();
            const GLFWvidmode *mode = glfwGetVideoMode(mon);
            glfwSetWindowMonitor(m_handle, nullptr, pos.x, pos.y, size.width,
                                 size.height, mode->refreshRate);
        }
    }
}

void window_client::redraw() {
    if (m_handle != nullptr) {
        glfwSwapBuffers(m_handle);
    }
}

bool window_client::should_close() const {
    return m_handle == nullptr || glfwWindowShouldClose(m_handle) == GLFW_TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

using client_map_t = std::map<GLFWwindow *, window_client *>;

static client_map_t &client_map() noexcept {
    static std::map<GLFWwindow *, window_client *> map;
    return map;
}

static void assign_handle_client(GLFWwindow *h, window_client *c) noexcept {
    if (h == nullptr) {
        return;
    }

    auto &map = client_map();
    auto iter = map.find(h);
    map.insert_or_assign(iter, h, c);
}

static void erase_handle_client(GLFWwindow *h) noexcept {
    client_map().erase(h);
}

static window_client *find_client(GLFWwindow *h) noexcept {
    auto iter = client_map().find(h);
    return iter == client_map().end() ? nullptr : iter->second;
}

static tue::wsi::window_context *find_client_context(GLFWwindow *h) noexcept {
    if (auto *cli = find_client(h)) {
        return cli->context();
    }
    return nullptr;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void cb_error(int code, const char *what);
static void cb_refresh(GLFWwindow *h);
static void cb_fbsize(GLFWwindow *h, int width, int height);
static void cb_key(GLFWwindow *h, int key, int scancode, int action, int mods);
static void cb_mouse(GLFWwindow *h, int key, int action, int mods);
static void cb_cursor(GLFWwindow *h, double x, double y);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

window_system_client::window_system_client() {
    glfwSetErrorCallback(cb_error);
    glfwInit();
}

window_system_client::~window_system_client() {
    glfwTerminate();
}

void window_system_client::create_handle(window_client &wc) {
    auto *ctx = wc.context();

    auto size = (ctx != nullptr) ? ctx->get_normal_size()
                                 : tue::wsi::window_size{200, 200};
    auto mode = (ctx != nullptr) ? ctx->get_default_mode()
                                 : tue::wsi::window_mode::normal;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWmonitor *mon = nullptr;
    if (mode == tue::wsi::window_mode::fullscreen) {
        mon = glfwGetPrimaryMonitor();
    }

    GLFWwindow *h = glfwCreateWindow(size.width, size.height, "", mon, nullptr);
    if (h == nullptr) {
        return;
    }

    glfwSetWindowRefreshCallback(h, cb_refresh);
    glfwSetFramebufferSizeCallback(h, cb_fbsize);
    glfwSetKeyCallback(h, cb_key);
    glfwSetMouseButtonCallback(h, cb_mouse);
    glfwSetCursorPosCallback(h, cb_cursor);

    if (GLFWwindow *old = wc.set_native_handle(h)) {
        glfwDestroyWindow(old);
    }

    assign_handle_client(h, &wc);

    glfwShowWindow(h);
}
void window_system_client::destroy_handle(window_client &wc) {
    if (GLFWwindow *old = wc.set_native_handle(nullptr)) {
        glfwDestroyWindow(old);
        erase_handle_client(old);
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void cb_error(int code, const char *what) {
    std::println(stderr, "[glfw] error[{}]: {}", code, what);
}

static void cb_refresh(GLFWwindow *h) {
    if (auto *ctx = find_client_context(h)) {
        ctx->on_event(tue::wsi::redraw_event{});
    }
}

static void cb_fbsize(GLFWwindow *h, int width, int height) {
    if (auto *ctx = find_client_context(h)) {
        ctx->on_event(tue::wsi::resize_event{.size = {width, height}});
    }
}

static void cb_key(GLFWwindow *h, int key, [[maybe_unused]] int scancode,
                   int action, int mods) {
    if (auto *ctx = find_client_context(h)) {
        ctx->on_event(tue::wsi::keyboard_event{
            .code = key_code_from_glfw(key),
            .act = key_action_from_glfw(action),
            .mods = key_mods_from_glfw(mods),
        });
    }
}

static void cb_mouse(GLFWwindow *h, int key, int action, int mods) {
    if (auto *ctx = find_client_context(h)) {
        tue::wsi::mouse_event::position pos{};
        glfwGetCursorPos(h, &pos.x, &pos.y);
        ctx->on_event(tue::wsi::mouse_event{
            .code = key_code_from_glfw(key),
            .act = key_action_from_glfw(action),
            .mods = key_mods_from_glfw(mods),
            .pos = pos,
        });
    }
}

static void cb_cursor(GLFWwindow *h, double x, double y) {
    if (auto *ctx = find_client_context(h)) {
        ctx->on_event(tue::wsi::mouse_event{.pos = {x, y}});
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static tue::wsi::key_code key_code_from_glfw(int key) noexcept {
    using tue::wsi::key_code;

    constexpr auto ascii_upper = 128;
    if (key >= 0 && key < ascii_upper) {
        auto c = tue::wsi::key_code_from_ascii(key);
        if (c != key_code::unknown) {
            return c;
        }
    }

    // or do manual mapping

    switch (key) {
    case GLFW_KEY_SPACE:
        return key_code::space;
    case GLFW_KEY_ESCAPE:
        return key_code::escape;
    case GLFW_KEY_ENTER:
        return key_code::enter;

    case GLFW_MOUSE_BUTTON_LEFT:
        return key_code::mouse_left;
    case GLFW_MOUSE_BUTTON_RIGHT:
        return key_code::mouse_right;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        return key_code::mouse_mid;
    }

    return key_code::unspec;
}

static tue::wsi::key_action key_action_from_glfw(int action) noexcept {
    using tue::wsi::key_action;

    switch (action) {
    case GLFW_PRESS:
        return key_action::pressed;
    case GLFW_RELEASE:
        return key_action::released;
    }

    return key_action::unspec;
}

static tue::wsi::key_mods key_mods_from_glfw(int mods) noexcept {
    using tue::wsi::key_mods;

    key_mods mod{};
    mod.bits |= ((mods & GLFW_MOD_CONTROL) != 0) ? key_mods::ctrl : 0;
    mod.bits |= ((mods & GLFW_MOD_SHIFT) != 0) ? key_mods::shift : 0;
    mod.bits |= ((mods & GLFW_MOD_ALT) != 0) ? key_mods::alt : 0;
    return mod;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

std::unique_ptr<window_system_client> make_window_system_client() {
    return std::make_unique<window_system_client>();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace glfw
