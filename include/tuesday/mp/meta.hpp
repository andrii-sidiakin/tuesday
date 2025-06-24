#ifndef _TUE_MP_META_HPP_INCLUDED_
#define _TUE_MP_META_HPP_INCLUDED_

#include <concepts>

namespace tue::mp {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//

template <auto Id> struct meta_info {
  private:
    template <auto X> friend consteval auto make_meta(meta_info<X>);
};

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

  protected:
    constexpr static void id() {}

    friend meta_info<meta::id>;

    friend consteval auto make_meta(meta_info<meta::id> /*mi*/) {
        return meta{};
    }

  public:
    static consteval auto index() { return meta::id; };
};

///
template <typename T> meta(T) -> meta<T>;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
template <typename T> constexpr meta<T> meta_for{};

///
using meta_index_t = decltype(meta<void>::index());

static_assert(std::same_as<meta_index_t, decltype(meta<char>::index())>);
static_assert(std::same_as<meta_index_t, decltype(meta<int>::index())>);

///
template <class T> inline constexpr auto meta_index = meta<T>::index();

template <auto Id>
using type_of = typename decltype(make_meta(meta_info<Id>{}))::type;

static_assert(meta<int>::index() == meta<int>::index());
static_assert(meta<char>::index() != meta<int>::index());
static_assert(meta<char>::index() == meta_index<char>);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::mp

#endif
