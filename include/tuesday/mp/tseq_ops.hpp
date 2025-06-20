#ifndef _TUE_MP_TSEQ_OPS_HPP_INCLUDED_
#define _TUE_MP_TSEQ_OPS_HPP_INCLUDED_

#include <tuesday/mp/tseq.hpp>

namespace tue::mp {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.mp.tseq.unique

/// removes duplicates so that each T appears only once
struct unique_op {

    template <class A, class B> struct impl;

    template <class... Bs> struct impl<tseq<>, tseq<Bs...>> {
        using type = tseq<Bs...>;
    };

    template <class A, class... As, class... Bs>
    struct impl<tseq<A, As...>, tseq<Bs...>> {
        using type = std::conditional_t<
            (... or std::same_as<A, Bs>), // already present?
            typename impl<tseq<As...>, tseq<Bs...>>::type,
            typename impl<tseq<As...>, tseq<Bs..., A>>::type>;
    };

    template <typename... As>
    consteval auto operator()(tseq<As...> a) const noexcept {
        return typename impl<tseq<As...>, tseq<>>::type{};
    }
};

inline constexpr unique_op unique{};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.mp.tseq.concat

/// concatenates given sequences into a single one preserving the order
struct concat_op {

    template <typename... As, typename... Bs>
    consteval auto operator()(tseq<As...> /*a*/,
                              tseq<Bs...> /*b*/) const noexcept {
        return tseq<As..., Bs...>{};
    }

    template <typename... As, typename B>
    consteval auto operator()(tseq<As...> /*a*/, B /*b*/) const noexcept {
        return tseq<As..., B>{};
    }
};

inline constexpr concat_op concat{};

template <typename... As, typename B>
consteval auto operator+(tseq<As...> a, B b) noexcept {
    return concat(a, b);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.mp.tseq.join

struct join_op {

    template <typename... As, typename... Bs>
    consteval auto operator()(tseq<As...> a, tseq<Bs...> b) const noexcept {
        return unique(concat(a, b));
    }

    template <typename... As, typename B>
    consteval auto operator()(tseq<As...> a, B b) const noexcept {
        return unique(concat(a, b));
    }
};

inline constexpr join_op join{};

template <typename... As, typename B>
consteval auto operator|(tseq<As...> a, B b) noexcept {
    return join(a, b);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::mp

#endif
