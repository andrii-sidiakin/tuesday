#ifndef _TUE_WSI_WINDOW_EVENTS_HPP_INCLUDED_
#define _TUE_WSI_WINDOW_EVENTS_HPP_INCLUDED_

#include <tuesday/wsi/keyboard.hpp>
#include <tuesday/wsi/window_attrs.hpp>

namespace tue::wsi {

///
struct resize_event {
    window_size size{};
};

///
struct redraw_event {
    //
};

struct keyboard_event {
    key_code code{key_code::unspec};
    key_action act{key_action::unspec};
    key_mods mods{key_mods::unspec};

    constexpr bool pressed(key_code c) const noexcept {
        return act == key_action::pressed && code == c;
    }

    constexpr bool released(key_code c) const noexcept {
        return act == key_action::released && code == c;
    }
};

struct mouse_event {
    key_code code{key_code::unspec};
    key_action act{key_action::unspec};
    key_mods mods{key_mods::unspec};

    struct position {
        double x{};
        double y{};
    } pos{};

    constexpr bool pressed(key_code c) const noexcept {
        return act == key_action::pressed && code == c;
    }

    constexpr bool released(key_code c) const noexcept {
        return act == key_action::released && code == c;
    }

    constexpr bool holds(std::uint8_t modbits) const noexcept {
        return (modbits & mods.bits) != 0;
    }
};

} // namespace tue::wsi

#endif
