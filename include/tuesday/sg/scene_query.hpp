#ifndef _TUE_SG_SCENE_QUERY_HPP_INCLUDED_
#define _TUE_SG_SCENE_QUERY_HPP_INCLUDED_

#include <tuesday/mp.hpp>

#include <variant>

namespace tue::sg {

namespace details {

template <class F> struct do_exec_for;
template <class Impl, class F, class Fs> struct do_exec_impl_for;

template <class Fs> struct feature_query;
template <class Impl, class Fs> struct do_exec_crtp;

///
///
template <class F> struct do_exec_for {
    virtual void do_exec(F) = 0;

  protected:
    ~do_exec_for() = default;
};

///
///
template <> struct do_exec_for<void> {
    void *m_impl_ptr{};
};

///
///
template <class... Fs>
struct do_exec_for<mp::tseq<Fs...>> : do_exec_for<void>, do_exec_for<Fs>... {
    using do_exec_for<Fs>::do_exec...;

    constexpr void exec(std::variant<Fs...> v) {
        std::visit([this]<class F>(F f) { this->do_exec(f); }, v);
    }
};

///
///
template <class Impl, class F, class Fs>
struct do_exec_impl_for : virtual do_exec_for<Fs> {
    static_assert(std::is_class_v<Impl>, "Impl must be a class");

    using do_exec_for<Fs>::do_exec;

    void do_exec(F f) final {
        static_cast<Impl *>(do_exec_for<void>::m_impl_ptr)->on_exec(f);
    }
};

///
///
template <class Impl, class... Fs>
struct do_exec_crtp<Impl, mp::tseq<Fs...>>
    : do_exec_impl_for<Impl, Fs, mp::tseq<Fs...>>... {
    static_assert(std::is_class_v<Impl>, "Impl must be a class");

    using do_exec_impl_for<Impl, Fs, mp::tseq<Fs...>>::do_exec...;

    do_exec_crtp() {
        static_assert(std::derived_from<Impl, do_exec_crtp>,
                      "Impl must derive do_exec_crtp");
        do_exec_for<mp::tseq<Fs...>>::m_impl_ptr = static_cast<Impl *>(this);
    }
};

///
///
template <class Impl, class Cmd, class Fs>
struct do_exec_cmd : do_exec_crtp<do_exec_cmd<Impl, Cmd, Fs>, Fs> {

    using cmd_t = std::conditional_t<std::is_lvalue_reference_v<Cmd>, Cmd &,
                                     std::remove_cvref_t<Cmd>>;

    cmd_t m_cmd;

    explicit constexpr do_exec_cmd(Cmd &cmd) noexcept
        requires(std::is_lvalue_reference_v<Cmd>)
        : m_cmd{cmd} {}

    explicit constexpr do_exec_cmd(Cmd &&cmd) noexcept
        requires(not std::is_lvalue_reference_v<Cmd>)
        : m_cmd{std::move(cmd)} {}

    template <typename F> constexpr void on_exec(F f) {
        static_cast<Impl &>(*this).exec_cmd(m_cmd, f);
    }
};

} // namespace details

using details::do_exec_cmd;
using details::do_exec_for;

} // namespace tue::sg

#endif
