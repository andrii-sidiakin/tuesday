#ifndef _TUE_MP_HPP_INCLUDED_
#define _TUE_MP_HPP_INCLUDED_

#include <tuesday/mp/tseq.hpp>
#include <tuesday/mp/tseq_ops.hpp>

#include <concepts>

namespace tue::mp {

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

} // namespace tue::mp

#endif
