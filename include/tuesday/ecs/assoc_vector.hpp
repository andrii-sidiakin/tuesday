#ifndef _TUE_ECS_ASSOC_VECTOR_HPP_INCLUDED_
#define _TUE_ECS_ASSOC_VECTOR_HPP_INCLUDED_

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

        // using _pair_t = std::pair<std::remove_cvref_t<KeyRef>,
        //                           std::remove_cvref_t<ValueRef>>;
        //
        // using _ref_pair_t = std::pair<std::remove_cvref_t<KeyRef> &,
        //                               std::remove_cvref_t<ValueRef> &>;
        //
        // using _const_ref_pair_t =
        //     std::pair<std::remove_cvref_t<KeyRef> const &,
        //               std::remove_cvref_t<ValueRef> const &>;
        //
        // pair_of_key_val_refs() = delete;
        //
        // constexpr pair_of_key_val_refs(KeyRef kr, ValueRef vr) noexcept
        //     : first{kr}, second{vr} {}
        //
        // constexpr pair_of_key_val_refs(const pair_of_key_val_refs &p)
        // noexcept
        //     : first{p.first}, second{p.second} {}
        //
        // constexpr pair_of_key_val_refs(pair_of_key_val_refs &&p) noexcept
        //     : first{p.first}, second{p.second} {}
        //
        // const pair_of_key_val_refs &
        // operator=(const pair_of_key_val_refs &p) const noexcept {
        //     first = p.first;
        //     second = p.second;
        //     return *this;
        // }
        //
        // const pair_of_key_val_refs &
        // operator=(pair_of_key_val_refs &&p) const noexcept {
        //     first = p.first;
        //     second = p.second;
        //     return *this;
        // }
        //
        // operator _pair_t() const { return {first, second}; }
        // operator _ref_pair_t() const { return {first, second}; }
        // operator _const_ref_pair_t() const { return {first, second}; }
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

///
///
template <class Key, class Value> class assoc_vector {
  public:
    using key_type = Key;
    using value_type = std::pair<const Key, const Value &>;
    using mapped_type = Value;

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

    struct kr_pair {
        index_type ki{}; // pos in keys
        index_type ri{}; // ref index
    };

    struct vc_pair {
        index_type vi{}; // pos in values
        index_type rc{}; // reference count
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
    template <std::same_as<key_type> K, std::same_as<mapped_type> V>
    auto insert(K &&k, V &&v) {
        auto [ki, ri] = insert_kv(std::forward<K>(k), std::forward<V>(v));
        auto [ii, ok] = m_index.insert({m_keys[ki], kr_pair{ki, ri}});
        tue_assert(ok, "key already exists");
        return kr_pair{ki, ri};
    }

    template <std::same_as<key_type> K> auto insert(K &&k, kr_pair p) {
        auto ref_it = m_refs.find(p.ri);
        if (ref_it == m_refs.end()) {
            throw std::out_of_range("unknown value reference");
        }

        index_type ki = m_keys.size();
        m_keys.emplace_back(std::forward<K>(k));

        auto [ii, ok] = m_index.insert({m_keys[ki], kr_pair{ki, p.ri}});
        if (not tue_assert(ok, "key already exists")) {
            m_keys.pop_back();
            throw std::logic_error("key already exists"); // TODO
        }

        ref_it->second.rc += 1;

        return kr_pair{ki, p.ri};
    }

    auto erase(const key_type &k) {
        auto it = m_index.find(k);
        if (it != m_index.end()) {
            erase_kv(it->second);
            m_index.erase(it);
        }
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

    template <std::same_as<mapped_type> V> auto insert_kv(key_type k, V &&v) {
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
