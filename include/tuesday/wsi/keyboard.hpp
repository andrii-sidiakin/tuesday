#ifndef _TUE_WSI_KEYBOARD_HPP_INCLUDED_
#define _TUE_WSI_KEYBOARD_HPP_INCLUDED_

#include <cstdint>
#include <limits>

namespace tue::wsi {

///
enum class key_code : std::uint16_t {
    unspec = 0,

    // clang-format off
    num_0 = '0',
    num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, 
    num_9 = '9',
    // clang-format on

    // clang-format off
    A = 'A',
    B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, 
    Z = 'Z',
    // clang-format on

    space,
    escape,
    enter,

    //
    mouse_left,
    mouse_mid,
    mouse_right,

    unknown = std::numeric_limits<std::uint16_t>::max(),
};

///
enum class key_action : std::uint8_t {
    unspec = 0,
    pressed,
    released,
};

///
struct key_mods {
    enum mod_bits : std::uint8_t {
        ctrl = (1 << 0),
        shift = (1 << 1),
        alt = (1 << 2),
    };

    static constexpr std::uint8_t unspec{0};

    std::uint8_t bits{unspec};
};

extern key_code key_code_from_ascii(char c) noexcept;

} // namespace tue::wsi

#endif
