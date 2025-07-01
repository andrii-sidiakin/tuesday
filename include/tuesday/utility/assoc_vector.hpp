#ifndef _TUE_ASSOC_VECTOR_HPP_INCLUDED_
#define _TUE_ASSOC_VECTOR_HPP_INCLUDED_

#include <tuesday/assert.hpp>

#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace tue::ecs {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.ecs.assoc_vector

///
///
template <class KeyRef, class ValueRef, class AssocVector, class KeyIter>
class assoc_vector_iterator {

    static_assert(std::is_reference_v<KeyRef> && std::is_reference_v<ValueRef>);

    struct kv_refs {
        KeyRef first;
        ValueRef second;
    };

  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type =
        std::pair<std::remove_cvref_t<KeyRef>, std::remove_cvref_t<ValueRef>>;
    using reference = kv_refs;
    using difference_type = std::iterator_traits<KeyIter>::difference_type;

  public:
    assoc_vector_iterator(AssocVector &vec, KeyIter key_it) noexcept
        : m_vec{&vec}, m_key_it{std::move(key_it)} {}

    reference operator*() const {
        return reference{*m_key_it, m_vec->operator[](*m_key_it)};
    }

    assoc_vector_iterator &operator++() & {
        ++m_key_it;
        return *this;
    }

    assoc_vector_iterator operator++(int) {
        auto iter{*this};
        ++(*this);
        return iter;
    }

    friend constexpr bool operator!=(const assoc_vector_iterator &a,
                                     const assoc_vector_iterator &b) noexcept {
        return a.m_key_it != b.m_key_it;
    }

  private:
    AssocVector *m_vec{nullptr};
    KeyIter m_key_it;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
///
template <class Key, class Value> class assoc_vector {
  public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;

    using key_container = std::pmr::vector<key_type>;

    using iterator =
        assoc_vector_iterator<const key_type &, mapped_type &, assoc_vector,
                              typename key_container::const_iterator>;
    using const_iterator =
        assoc_vector_iterator<const key_type &, const mapped_type &,
                              const assoc_vector,
                              typename key_container::const_iterator>;

  private:
    using index_type = std::pmr::vector<Value>::size_type;

    static constexpr index_type invalid_index{static_cast<index_type>(-1)};

    struct kr_pair {
        index_type ki{invalid_index}; // pos in keys
        index_type ri{invalid_index}; // ref index

        constexpr explicit operator bool() const noexcept {
            return ki != invalid_index && ri != invalid_index;
        }
    };

    struct vc_pair {
        index_type vi{invalid_index}; // pos in values
        index_type rc{0};             // reference count

        constexpr explicit operator bool() const noexcept {
            return vi != invalid_index;
        }
    };

  public:
    assoc_vector() = default;
    assoc_vector(const assoc_vector &) = default;
    assoc_vector &operator=(const assoc_vector &) = default;
    assoc_vector(assoc_vector &&) noexcept = default;
    assoc_vector &operator=(assoc_vector &&) noexcept = default;

  public:
    constexpr bool empty() const noexcept { return m_keys.empty(); }
    constexpr auto size() const noexcept { return m_keys.size(); }

    constexpr auto &keys() const noexcept { return m_keys; }
    constexpr auto &values() const noexcept { return m_vals; }

    constexpr auto &mutable_values() & noexcept { return m_vals; }

  public:
    auto begin() noexcept { return iterator{*this, m_keys.begin()}; }
    auto end() noexcept { return iterator{*this, m_keys.end()}; }
    auto begin() const noexcept {
        return const_iterator{*this, m_keys.begin()};
    }
    auto end() const noexcept { return const_iterator{*this, m_keys.end()}; }

  public:
    ///
    template <typename... As>
        requires(std::constructible_from<mapped_type, As...>)
    auto emplace(const key_type &k, As &&...as) {
        std::pair<key_type, mapped_type> p{
            std::piecewise_construct, std::forward_as_tuple(k),
            std::forward_as_tuple(std::forward<As>(as)...)};
        return emplace_impl(std::move(p.first), std::move(p.second));
    }

    ///
    template <typename... As>
        requires(std::constructible_from<mapped_type, As...>)
    auto emplace(key_type &&k, As &&...as) {
        std::pair<key_type, mapped_type> p{
            std::piecewise_construct, std::forward_as_tuple(std::move(k)),
            std::forward_as_tuple(std::forward<As>(as)...)};
        return emplace_impl(std::move(p.first), std::move(p.second));
    }

    ///
    template <typename M>
        requires(std::constructible_from<mapped_type, M>)
    auto insert(const key_type &k, M &&m) {
        return emplace(k, mapped_type(std::forward<M>(m)));
    }

    ///
    template <typename M>
        requires(std::constructible_from<mapped_type, M>)
    auto insert(key_type &&k, M &&m) {
        return emplace(std::move(k), mapped_type(std::forward<M>(m)));
    }

    ///
    auto insert_shared(const key_type &k, kr_pair kr) {
        return insert_shared_impl(k, kr);
    }

    ///
    auto insert_shared(key_type &&k, kr_pair kr) {
        return insert_shared_impl(std::move(k), kr);
    }

    ///
    auto insert_shared(const key_type &k, const key_type &src) {
        return insert_shared(k, m_index.at(src));
    }

    ///
    auto insert_shared(key_type &&k, const key_type &src) {
        return insert_shared(std::move(k), m_index.at(src));
    }

    ///
    bool erase(const key_type &k) {
        auto it = m_index.find(k);
        if (it != m_index.end()) {
            erase_kv(it->second);
            m_index.erase(it);
            return true;
        }
        return false;
    }

    auto erase(const_iterator iter) { return erase(iter->first); }

    auto erase(kr_pair kr) {
        tue_assert(kr, "invalid reference");
        tue_assert(kr.ki < m_keys.size(), "invalid reference");
        return erase(m_keys[kr.ki]);
    }

  public:
    constexpr const mapped_type &at(const key_type &k) const {
        return m_vals[get_value_index(k)];
    }

    template <class Self>
    constexpr auto &operator[](this Self &self, const key_type &k) {
        return self.m_vals[self.get_value_index(k)];
    }

  private:
    constexpr index_type get_value_index(const key_type &k) const {
        return m_refs.at(m_index.at(k).ri).vi;
    }

    kr_pair emplace_impl(key_type k, mapped_type &&v) {
        auto [ii, ok] = m_index.insert({std::move(k), {}});
        if (!ok) {
            return {};
        }
        try {
            auto kr = insert_kv(ii->first, std::move(v));
            ii->second = kr;
            return kr;
        }
        catch (...) {
            m_index.erase(ii);
            throw;
        }
        return {};
    }

    kr_pair insert_shared_impl(key_type k, kr_pair kr) {
        auto ref_it = m_refs.find(kr.ri);
        if (ref_it == m_refs.end()) {
            throw std::out_of_range("unknown value reference");
        }

        auto [ii, ok] = m_index.insert({std::move(k), kr_pair{}});
        if (!ok) {
            return {};
        }
        try {
            index_type ki = m_keys.size();
            m_keys.emplace_back(ii->first);
            ii->second = kr_pair{ki, kr.ri};
            ref_it->second.rc += 1;
            return kr_pair{ki, kr.ri};
        }
        catch (...) {
            m_index.erase(ii);
            throw;
        }
        return {};
    }

    template <typename V> auto insert_kv(key_type k, V &&v) {
        static_assert(std::constructible_from<mapped_type, V>);

        index_type ki = m_keys.size();
        index_type ri = m_vals.size();
        index_type vi = ri;

        // insert ref entry with 0 count meaning it's not complete yet
        auto [ref_it, ref_ok] = m_refs.insert({ri, vc_pair{vi, 0}});

        try {
            m_keys.emplace_back(std::move(k));
        }
        catch (...) {
            m_refs.erase(ref_it);
            throw;
        }

        try {
            m_vals.emplace_back(std::forward<V>(v));
        }
        catch (...) {
            m_keys.pop_back();
            m_refs.erase(ref_it);
            throw;
        }

        ref_it->second = vc_pair{vi, 1}; // complete insertion

        return kr_pair{ki, ri};
    }

    auto erase_kv(kr_pair p) {
        index_type ki_last = m_keys.size() ? m_keys.size() - 1 : p.ki;
        if (ki_last != p.ki) {
            m_index[m_keys[ki_last]].ki = p.ki;
            m_keys[p.ki] = std::move(m_keys[ki_last]);
        }
        m_keys.pop_back();

        auto ref_it = m_refs.find(p.ri);
        tue_assert(ref_it != m_refs.end(), "unknown value reference");
        tue_assert(ref_it->second.rc != 0, "element is not ready yet");
        if (ref_it->second.rc > 1) {
            ref_it->second.rc -= 1;
        }
        else {
            index_type vi = ref_it->second.vi;
            index_type vi_last = m_vals.size() ? m_vals.size() - 1 : vi;
            if (vi_last != vi) {
                tue_assert(m_refs.contains(vi_last), "unknown value reference");
                m_refs[vi_last] = vc_pair{vi, ref_it->second.rc};
                m_vals[vi] = std::move(m_vals[vi_last]);
            }
            m_vals.pop_back();
            m_refs.erase(ref_it);
        }
    }

  private:
    std::pmr::vector<key_type> m_keys;
    std::pmr::vector<mapped_type> m_vals;
    std::pmr::unordered_map<key_type, kr_pair> m_index;
    std::pmr::unordered_map<index_type, vc_pair> m_refs;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::ecs

#endif
