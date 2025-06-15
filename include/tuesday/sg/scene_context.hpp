#ifndef _TUE_SG_SCENE_CONTEXT_HPP_INCLUDED_
#define _TUE_SG_SCENE_CONTEXT_HPP_INCLUDED_

#include <tuesday/sg/scene_object.hpp>

#include <tuesday/mp.hpp>

#include <map>
#include <tuple>
#include <vector>

namespace tue::sg {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class Fs> struct features_map;

template <class Fs> struct features_map_key;

template <class Fs> using features_map_key_t = features_map_key<Fs>::type;

///
///
template <class E, class... Fs> struct features_map<mp::tseq<E, Fs...>> {
    using key_type = features_map_key_t<mp::tseq<E, Fs...>>;

    template <class F> struct data_set {
        using data_vector = std::vector<F>;
        using key_map = std::map<key_type, typename data_vector::size_type>;

        key_map map;
        data_vector vec;
    };

    std::tuple<data_set<Fs>...> tup;

    template <class F>
    constexpr auto &get_data_set(this auto &&self,
                                 mp::meta<F> mt = mp::meta_for<F>) noexcept {
        static_assert(mp::tseq<Fs...>::has(mt), "Unknown feature");
        return std::get<data_set<F>>(
            std::forward_like<decltype(self)>(self.tup));
    }

    template <class F> auto insert_or_assign(key_type key, F &&val) {
        auto &data = get_data_set(mp::meta_for<F>);
        auto [iter, emplaced] = data.map.try_emplace(key, data.vec.size());
        if (emplaced) {
            try {
                data.vec.emplace_back(std::forward<F>(val));
            }
            catch (...) {
                data.map.erase(iter);
                throw;
            }
        }
        else {
            data.vec[iter->second] = std::forward<F>(val);
        }
    }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class Fs, class Cs> class basic_scene_context;

///
///
template <class... Fs, class Cs>
class basic_scene_context<mp::tseq<Fs...>, Cs> {
  public:
    using feature_tseq = mp::tseq<Fs...>;
    using object = basic_object<feature_tseq, Cs>;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::sg

#endif
