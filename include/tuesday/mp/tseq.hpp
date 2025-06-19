#ifndef _TUE_MP_TSEQ_HPP_INCLUDED_
#define _TUE_MP_TSEQ_HPP_INCLUDED_

#include <tuesday/mp/meta.hpp>

namespace tue::mp {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.mp.tseq

/// 
/// sequence of types
///
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
