#ifndef _TUE_WSI_WINDOW_ATTRS_HPP_INCLUDED_
#define _TUE_WSI_WINDOW_ATTRS_HPP_INCLUDED_

#include <cstdint>
#include <utility>

namespace tue::wsi {

///
enum class window_mode : std::uint8_t {
    normal,
    fullscreen,
};

///
struct window_pos {
    int x{};
    int y{};
};

///
struct window_size {
    int width{};
    int height{};
};

///
struct window_attrs {
    window_pos pos{};
    window_size size{};
    window_mode mode{window_mode::normal};

    void set(window_pos val) { pos = val; }
    void set(window_size val) { size = val; }
    void set(window_mode val) { mode = val; }

    template <class Attr> window_attrs &with(Attr &&val) {
        set(std::forward<Attr>(val));
        return *this;
    }
};

} // namespace tue::wsi

#endif
