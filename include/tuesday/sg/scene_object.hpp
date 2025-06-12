#ifndef _TUE_SG_SCENE_OBJECT_HPP_INCLUDED_
#define _TUE_SG_SCENE_OBJECT_HPP_INCLUDED_

#include <tuesday/sg/scene_query.hpp>

#include <tuesday/mp.hpp>

#include <tuple>

namespace tue::sg {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class Fs, class Cs> class basic_object;
template <typename T, class Fs, class Cs> class basic_object_impl;

template <typename T, class Fs> struct object_feature_traits;

template <typename T, class Fs>
using object_feature_tseq = object_feature_traits<T, Fs>::feature_tseq;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace details {

struct object_base {

    template <class Obj, class F> struct feature_query_arg {
        static_assert(std::is_base_of_v<object_base, Obj>,
                      "Obj must derive object_base");

        Obj *obj{};
        F *fp{};
    };

    template <class Obj, class... Fs>
    using feature_query_arg_seq = mp::tseq<feature_query_arg<Obj, Fs>...>;

    template <class Cmd, class Obj, class... Fs>
    struct feature_query_cmd
        : do_exec_cmd<feature_query_cmd<Cmd, Obj, Fs...>, Cmd,
                      feature_query_arg_seq<Obj, Fs...>> {

        // re-use base class constructor
        using do_exec_cmd<feature_query_cmd<Cmd, Obj, Fs...>, Cmd,
                          feature_query_arg_seq<Obj, Fs...>>::do_exec_cmd;

        // command executor
        template <class CmdT, class F>
        constexpr void exec_cmd(CmdT &cmd, feature_query_arg<Obj, F> arg) {
            if constexpr (std::invocable<CmdT, Obj &, F *>) {
                cmd(*arg.obj, arg.fp);
            }
            else if constexpr (std::invocable<CmdT, F *>) {
                cmd(arg.fp);
            }
            else {
                static_assert(std::invocable<CmdT>, "Cannot invoke");
            }
        }
    };
};

} // namespace details

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
///
template <class E, class... Fs, class... Cs>
class basic_object<mp::tseq<E, Fs...>, mp::tseq<Cs...>>
    : private details::object_base, public E {
  protected:
    using context_fseq = mp::tseq<Fs...>;
    using feature_ptr = std::variant<Fs *...>;

    template <class F>
    using feature_query_arg = object_base::feature_query_arg<basic_object, F>;

    using feature_query_arg_seq =
        object_base::feature_query_arg_seq<basic_object, Fs...>;

    using feature_query = do_exec_for<feature_query_arg_seq>;

    template <class Cmd>
    using feature_query_cmd =
        object_base::feature_query_cmd<Cmd, basic_object, Fs...>;

  public:
    virtual ~basic_object() = default;

  public:
    ///
    virtual void query(feature_query &q) = 0;

    ///
    virtual void set_feature_ptr(feature_ptr fp) = 0;

    template <class Cmd> constexpr void apply(Cmd &&cmd) {
        feature_query_cmd<Cmd> q{std::forward<Cmd>(cmd)};
        query(q);
    }

  public:
    ///
    constexpr auto &entity(this auto &self) noexcept {
        return std::forward_like<decltype(self)>(self.m_entity);
    }

  private:
    E m_entity;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace details {

template <class Fs> struct feature_ptr_tuple;

template <class... Fs> struct feature_ptr_tuple<mp::tseq<Fs...>> {
    std::tuple<Fs *...> tup;
};

} // namespace details

///
/// basic_object implementation for type T
///
template <typename T, class... Fs, class... Cs>
class basic_object_impl<T, mp::tseq<Fs...>, mp::tseq<Cs...>> final
    : public basic_object<mp::tseq<Fs...>, mp::tseq<Cs...>> {
  private:
    using base_type = basic_object<mp::tseq<Fs...>, mp::tseq<Cs...>>;

    template <class F>
    using feature_query_arg = base_type::template feature_query_arg<F>;

    using feature_query = base_type::feature_query;

    using context_fseq = base_type::context_fseq;
    using feature_ptr = base_type::feature_ptr;

    using fptr_tuple = details::feature_ptr_tuple<context_fseq>;

    using value_type = T;
    using object_fseq = object_feature_tseq<value_type, mp::tseq<Fs...>>;

  public:
    constexpr basic_object_impl() noexcept(
        std::is_nothrow_default_constructible_v<value_type>) = default;

    constexpr basic_object_impl(const basic_object_impl &) noexcept(
        std::is_nothrow_copy_constructible_v<value_type>) = default;

    constexpr basic_object_impl &operator=(const basic_object_impl &) noexcept(
        std::is_nothrow_copy_assignable_v<value_type>) = default;

    constexpr basic_object_impl(basic_object_impl &&) noexcept(
        std::is_nothrow_move_constructible_v<value_type>) = default;

    constexpr basic_object_impl &operator=(basic_object_impl &&) noexcept(
        std::is_nothrow_move_assignable_v<value_type>) = default;

    ///
    explicit constexpr basic_object_impl(const value_type &value) noexcept(
        std::is_nothrow_copy_constructible_v<value_type>)
        : m_value{value} {}

    ///
    explicit constexpr basic_object_impl(value_type &&value) noexcept(
        std::is_nothrow_move_constructible_v<value_type>)
        : m_value{std::move(value)} {}

  private:
    constexpr void exec_query(feature_query & /*q*/,
                              mp::tseq<> /*unused*/) noexcept {}

    template <class G, class... Gs>
    constexpr void exec_query(feature_query &q, mp::tseq<G, Gs...> /*unused*/) {
        if constexpr (context_fseq::has(mp::meta_for<G>)) {
            q.exec(
                feature_query_arg<G>(this, get_feature_ptr(mp::meta_for<G>)));
        }
        exec_query(q, mp::tseq<Gs...>{});
    }

    constexpr void query(feature_query &q) override {
        exec_query(q, object_fseq{});
    }

  private:
    void set_feature_ptr(feature_ptr fp) override {
        std::visit(
            [this]<class F>(F *fp) {
                if constexpr (object_fseq::has(mp::meta_for<F>)) {
                    std::get<F *>(this->m_fptrs.tup) = fp;
                }
            },
            fp);
    }

    template <class F>
    auto *get_feature_ptr(this auto &&self, mp::meta<F> mt = {}) noexcept {
        if constexpr (object_fseq::has(mt)) {
            return std::get<F *>(self.m_fptrs.tup);
        }
        else {
            return (F *)nullptr;
        }
    }

  private:
    value_type m_value;
    fptr_tuple m_fptrs;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::sg

#endif
