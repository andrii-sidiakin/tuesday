#ifndef _TUE_MP_HPP_INCLUDED_
#define _TUE_MP_HPP_INCLUDED_

#include <concepts>

namespace tue::mp {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
    consteval friend bool operator==(meta<T> /*lhs*/,
                                     meta<U> /*rhs*/) noexcept {
        return std::same_as<T, U>;
    }
};

template <typename T> meta(T) -> meta<T>;

template <typename T> constexpr meta<T> meta_for{};

template <typename T>
consteval meta<T> &make_meta_for(T && /*unused*/) noexcept {
    return meta_for<T>;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/// sequence of signed integrals
template <std::signed_integral T, T... Is> struct iseq {};

/// sequence of unsigned integrals
template <std::unsigned_integral T, T... Us> struct useq {};

template <std::unsigned_integral T, T N> consteval auto make_useq() noexcept {
    return []<T... Vs>(this auto &self, useq<T, Vs...>) {
        if constexpr (sizeof...(Vs) < N) {
            return self(useq<T, Vs..., sizeof...(Vs)>{});
        }
        else {
            return useq<T, Vs...>{};
        }
    }(useq<T>{});
}

template <std::unsigned_integral T, T N>
constexpr auto useq_for = make_useq<T, N>();

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/// sequence of types
template <typename... Ts> struct tseq {

    static consteval std::size_t size() noexcept { return sizeof...(Ts); }

    static consteval std::size_t npos() noexcept { return size() + 1; };

    template <typename T>
    static consteval bool has(meta<T> m = meta_for<T>) noexcept {
        return (... or (m == meta_for<Ts>));
    }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::mp

#endif
