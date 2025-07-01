#ifndef _TUE_UTILITY_NONCOPYABLE_HPP_INCLUDED_
#define _TUE_UTILITY_NONCOPYABLE_HPP_INCLUDED_

namespace tue {

namespace _ns_noncopyable {

class noncopyable {
  protected:
    constexpr noncopyable(noncopyable &&) noexcept = default;
    constexpr noncopyable &operator=(noncopyable &&) noexcept = default;

  protected:
    constexpr noncopyable() noexcept = default;
    constexpr ~noncopyable() noexcept = default;
};

} // namespace _ns_noncopyable

using noncopyable = _ns_noncopyable::noncopyable;

} // namespace tue

#endif
