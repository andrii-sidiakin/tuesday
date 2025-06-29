#ifndef _TUE_GLFW_HPP_INCLUDED_
#define _TUE_GLFW_HPP_INCLUDED_

#include <tuesday/wsi/window_client.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utility>

namespace glfw {

class window_client;
class window_system_client;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class window_client : public tue::wsi::window_client {
  public:
    window_client(window_client &&other) noexcept;
    window_client &operator=(window_client &&other) noexcept;
    void swap(window_client &other) noexcept;

    explicit window_client(window_system_client &ws) noexcept;

    ~window_client();

    bool should_close() const override;
    void open() override;
    void close() override;
    void reload_attrs() override;
    void redraw() override;

  protected:
    friend class window_system_client;

    GLFWwindow *get_native_handle() const noexcept { return m_handle; }

    GLFWwindow *set_native_handle(GLFWwindow *handle) noexcept {
        return std::exchange(m_handle, handle);
    }

  private:
    window_system_client *m_wndsys{nullptr};
    GLFWwindow *m_handle{nullptr};
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class window_system_client : public tue::wsi::window_system_client {
  public:
    window_system_client();
    ~window_system_client();

    std::unique_ptr<tue::wsi::window_client> make_window_client() override {
        return std::make_unique<window_client>(*this);
    }

    void poll_events() override { glfwPollEvents(); }

    tue::wsi::gl_context make_gl_context() const noexcept override {
        return {.proc_addr = glfwGetProcAddress};
    }

    void make_current(tue::wsi::window_client &wc) noexcept override {
        auto *h = static_cast<window_client &>(wc).get_native_handle();
        if (h != nullptr) {
            glfwMakeContextCurrent(h);
        }
    }

  protected:
    friend class window_client;
    void create_handle(window_client &wc);
    void destroy_handle(window_client &wc);
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

std::unique_ptr<window_system_client> make_window_system_client();

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace glfw

#endif
