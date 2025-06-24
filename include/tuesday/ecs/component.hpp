#ifndef _TUE_ECS_COMPONENT_HPP_INCLUDED_
#define _TUE_ECS_COMPONENT_HPP_INCLUDED_

#include <tuesday/mp/tseq.hpp>
#include <tuesday/mp/tseq_ops.hpp>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace tue::ecs {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.ecs.component

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
///
///
template <class E> class component_storage_base {
  public:
    virtual ~component_storage_base() = default;
    virtual void erase(E) = 0;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
///
///
template <class E, class C>
class component_storage : public component_storage_base<E> {
  public:
    void insert(E e, C &&c) {
        auto slot = m_data.size();
        auto [it, ok] = m_index.insert({e, slot});
        if (ok) {
            m_back_index[slot] = std::move(e);
            m_data.emplace_back(std::move(c));
        }
    }

    void erase(E e) final {
        auto it = m_index.find(e);
        if (it == end(m_index)) {
            return;
        }

        auto slot = it->second;
        auto last = m_data.size() ? m_data.size() - 1 : slot;
        if (slot != last) {
            m_data[slot] = std::move(m_data[last]);
            m_index[m_back_index[last]] = slot;
            m_back_index[slot] = m_back_index[last];
            m_back_index.erase(last);
        }

        m_data.erase(m_data.begin() + slot);
    }

  public:
    constexpr C &operator[](E e) & { return m_data[m_index[e]]; }

    constexpr const C &operator[](E e) const & { return m_data[m_index[e]]; }

  private:
    std::map<E, std::size_t> m_index;
    std::unordered_map<std::size_t, E> m_back_index;
    std::vector<C> m_data;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
///
///
template <class E> class component_registry {
  public:
    using storage_type = component_storage_base<E>;
    template <class C> using storage_for = component_storage<E, C>;

  public:
    component_registry() = default;
    component_registry(component_registry &&) noexcept = default;
    component_registry &operator=(component_registry &&) noexcept = default;

  public:
    constexpr storage_type *find(mp::meta_index_t index) const noexcept {
        auto it = m_index.find(index);
        return it == end(m_index) ? nullptr : m_data[it->second].get();
    }

    template <class C> constexpr storage_for<C> *find() const noexcept {
        return static_cast<storage_for<C> *>(find(mp::meta_index<C>));
    }

    template <class C, typename... Args> storage_for<C> &make(Args &&...args) {
        auto [it, ok] = m_index.insert({mp::meta_index<C>, m_data.size()});

        if (!ok) {
            throw std::logic_error("Already registered");
        }

        m_data.emplace_back(
            std::make_unique<storage_for<C>>(std::forward<Args...>(args)...));

        return static_cast<storage_for<C> &>(*m_data.back());
    }

    template <class C> storage_for<C> &use() {
        if (auto *p = find<C>()) {
            return *p;
        }
        return make<C>();
    }

  public:
    template <class... Cs> auto insert(E e, Cs &&...cs) {
        static_assert(mp::is_unique(mp::tseq<Cs...>{}),
                      "Component list must be unique");
        (use<Cs>().insert(e, std::forward<Cs>(cs)), ...);
    }

    void erase(E e) {
        for (auto &c : m_data) {
            c->erase(e);
        }
    }

  private:
    std::unordered_map<mp::meta_index_t, std::size_t> m_index;
    std::vector<std::unique_ptr<storage_type>> m_data;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::ecs

#endif
