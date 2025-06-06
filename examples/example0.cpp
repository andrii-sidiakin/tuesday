#include <tuesday/wsi.hpp>

#include <glad/gl.h>

#include <print>

int main() {

    // make a connection to default wsi implementation
    auto wsi = tue::wsi::connect();
    // ask for a new window
    auto wnd = wsi.make_window();
    // ask for a event watcher
    auto *wcb = wnd.make_watcher();

    // setup callbacks

    wcb->on_size = [](auto &wnd, auto e) {
        glViewport(0, 0, e.size.width, e.size.height);
    };

    wcb->on_keyboard = [](auto &w, auto e) {
        using tue::wsi::key_code;
        if (e.pressed(key_code::escape)) {
            w.close();
        }
        else if (e.pressed(key_code::F)) {
            w.toggle_fullscreen();
        }
    };

    wcb->on_draw = [](auto &wnd) {
        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    };

    wcb->on_mouse = [](auto &wnd, auto e) {
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

    auto glx = wsi.make_gl_context();
    gladLoadGL(glx.proc_addr);

    while (!wnd.should_close()) {
        wsi.poll_events();
        wnd.swap_buffers();
    }

    return 0;
}
