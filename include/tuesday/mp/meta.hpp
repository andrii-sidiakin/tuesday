#ifndef _TUE_MP_META_HPP_INCLUDED_
#define _TUE_MP_META_HPP_INCLUDED_

#include <concepts>

namespace tue::mp {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.mp.meta

///
///
///
template <typename T> struct meta {
    using type = T;

    consteval meta() noexcept = default;
    consteval meta(const meta &) noexcept = default;
    consteval meta &operator=(const meta &) noexcept = default;
    consteval meta(meta &&) noexcept = default;
    consteval meta &operator=(meta &&) noexcept = default;

    template <std::same_as<T> U>
    explicit(false) consteval meta(U && /*unused*/) noexcept {}

    template <typename U>
    consteval friend bool operator==(meta<T> /*lhs*/, meta<U> /*rhs*/) {
        return std::same_as<T, U>;
    }

    template <typename U>
    consteval friend bool operator!=(meta<T> lhs, meta<U> rhs) {
        return !(lhs == rhs);
    }
};

///
template <typename T> meta(T) -> meta<T>;

///
template <typename T> constexpr meta<T> meta_for{};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::mp

#endif
