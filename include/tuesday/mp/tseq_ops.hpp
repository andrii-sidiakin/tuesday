#ifndef _TUE_MP_TSEQ_OPS_HPP_INCLUDED_
#define _TUE_MP_TSEQ_OPS_HPP_INCLUDED_

#include <tuesday/mp/tseq.hpp>

namespace tue::mp {

namespace details {

template <class, typename = void> struct as_tseq;

template <typename T> struct as_tseq<T, void> {
    using type = tseq<T>;
};

template <typename... Ts> struct as_tseq<tseq<Ts...>> {
    using type = tseq<Ts...>;
};

} // namespace details

template <typename T> using as_tseq_t = details::as_tseq<T>::type;

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

///
inline constexpr unique_op unique{};

///
template <typename... Ts>
using unique_result_t = decltype(unique(tseq<Ts...>{}));

///
template <typename... Ts>
consteval bool is_unique(tseq<Ts...> /*ts*/) noexcept {
    return std::same_as<tseq<Ts...>, unique_result_t<Ts...>>;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.mp.tseq.concat

/// concatenates given sequences into a single one preserving the order
struct concat_op {

    template <typename A, typename... Bs>
    consteval auto operator()(A /*a*/, Bs... /*bs*/) const noexcept {
        if constexpr (sizeof...(Bs)) {
            return concat_op{}(as_tseq_t<A>{}, as_tseq_t<Bs>{}...);
        }
        else {
            return as_tseq_t<A>{};
        }
    }

    template <typename... As, typename... Bs>
    consteval auto operator()(tseq<As...> /*as*/, Bs... /*bs*/) const noexcept {
        if constexpr (sizeof...(Bs)) {
            return concat_op{}(tseq<As...>{}, concat_op{}(as_tseq_t<Bs>{}...));
        }
        else {
            return tseq<As...>{};
        }
    }

    template <typename... As, typename... Bs>
    consteval auto operator()(tseq<As...> /*as*/,
                              tseq<Bs...> /*bs*/) const noexcept {
        return tseq<As..., Bs...>{};
    }
};

///
inline constexpr concat_op concat{};

///
template <typename... Ts>
using concat_result_t = decltype(concat(as_tseq_t<Ts>{}...));

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.mp.tseq.join_unique

///
struct join_unique_op {

    template <typename... As>
    consteval auto operator()(As... /*a*/) const noexcept {
        return unique(concat(as_tseq_t<As>{}...));
    }
};

///
inline constexpr join_unique_op join_unique{};

///
template <typename... Ts>
using join_unique_result_t = decltype(join_unique(as_tseq_t<Ts>{}...));

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//

///
template <typename... As, typename... Bs>
consteval auto operator+(tseq<As...> /*a*/, tseq<Bs...> /*b*/) noexcept {
    return concat(tseq<As...>{}, tseq<Bs...>{});
}

///
template <typename... As, typename... Bs>
consteval auto operator|(tseq<As...> /*a*/, tseq<Bs...> /*b*/) noexcept {
    return join_unique(tseq<As...>{}, tseq<Bs...>{});
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::mp

#endif
