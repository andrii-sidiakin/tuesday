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
            (... or std::same_as<A, Bs>),                  // already present?
            typename impl<tseq<As...>, tseq<Bs...>>::type, // skip
            typename impl<tseq<As...>, tseq<Bs..., A>>::type>; // append
    };

    template <typename... As>
    consteval auto operator()(tseq<As...> /*a*/) const noexcept {
        return typename impl<tseq<As...>, tseq<>>::type{};
    }
};

inline constexpr unique_op unique{};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.mp.tseq.concat

/// concatenates given sequences into a single one preserving the order
struct concat_op {

    template <typename... As>
    consteval auto operator()(tseq<As...> a) const noexcept {
        return a;
    }

    template <typename... As, typename... Bs, typename... Cs>
    consteval auto operator()(tseq<As...> a, tseq<Bs...> b,
                              Cs... cs) const noexcept {
        if constexpr (sizeof...(Cs) == 0) {
            return tseq<As..., Bs...>{};
        }
        else {
            return concat_op{}(a, concat_op{}(b, cs...));
        }
    }
};

inline constexpr concat_op concat{};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.mp.tseq.join_unique

///
struct join_unique_op {

    template <typename... As, typename... Bs>
    consteval auto operator()(tseq<As...> a, Bs... bs) const noexcept {
        return unique(concat(a, bs...));
    }
};

inline constexpr join_unique_op join_unique{};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//

template <typename... As, typename... Bs>
consteval auto operator+(tseq<As...> a, tseq<Bs...> b) noexcept {
    return concat(a, b);
}

template <typename... As, typename... Bs>
consteval auto operator|(tseq<As...> a, tseq<Bs...> b) noexcept {
    return join_unique(a, b);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::mp

#endif
