#ifndef _TUE_ECS_SYSTEM_HPP_INCLUDED_
#define _TUE_ECS_SYSTEM_HPP_INCLUDED_

#include <tuesday/mp/tseq.hpp>
#include <tuesday/mp/tseq_ops.hpp>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace tue::ecs {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.ecs.system

template <class S> struct system_feature_tseq;
template <class S> using system_feature_tseq_t = system_feature_tseq<S>::type;

template <class S> struct system_traits {
    using feature_tseq = system_feature_tseq_t<S>;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
///
template <class E> class system_base {
  public:
    virtual ~system_base() = default;

  public:
    constexpr auto &entities() & { return m_data; }
    constexpr const auto &entities() const & { return m_data; }

  public:
    constexpr void insert(E e) { m_data.emplace_back(std::move(e)); }

    constexpr void erase(E e) {
        auto it = std::ranges::find(m_data, e);
        if (it != end(m_data)) {
            *it = m_data.back();
            m_data.pop_back();
        }
    }

  private:
    std::vector<E> m_data;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
///
template <class Derived, class E> class basic_system : public system_base<E> {
  public:
    //
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
///
template <class Entity, class Kind> class system_registry {
  public:
    using entity_type = Entity;
    using system_kind = Kind;
    using system_type = system_base<Entity>;

  public:
    system_registry() = default;
    system_registry(system_registry &&) noexcept = default;
    system_registry &operator=(system_registry &&) noexcept = default;

  public:
    constexpr system_type *find(mp::meta_index_t index) const noexcept {
        auto it = m_index.find(index);
        return it == end(m_index) ? nullptr : m_data[it->second].get();
    }

    template <std::derived_from<system_type> S>
    constexpr S *find() const noexcept {
        return static_cast<S *>(find(mp::meta_index<S>));
    }

    template <std::derived_from<system_type> S, typename... Args>
    S &make(Args &&...args) {
        auto [it, ok] = m_index.insert({mp::meta_index<S>, m_data.size()});

        if (!ok) {
            throw std::logic_error("Already registered");
        }

        m_data.emplace_back(Kind::make(system_feature_tseq_t<S>{}),
                            std::make_unique<S>(std::forward<Args>(args)...));

        return static_cast<S &>(*m_data.back().ptr);
    }

    template <std::derived_from<system_type> S> S &use() {
        if (auto *s = find<S>()) {
            return *s;
        }
        return make<S>();
    }

  public:
    template <class EntityState> auto insert(Entity e, EntityState s) {
        for (auto &d : m_data) {
            if (d.kind.match(s)) {
                d.ptr->insert(e);
            }
        }
    }

    template <class EntityState> auto erase(Entity e, EntityState s) {
        for (auto &d : m_data) {
            if (d.kind.match(s)) {
                d.ptr->erase(e);
            }
        }
    }

    void erase(Entity e) {
        for (auto &d : m_data) {
            d.ptr->erase(e);
        }
    }

  private:
    struct system_data {
        system_kind kind;
        std::unique_ptr<system_type> ptr;
    };
    std::unordered_map<mp::meta_index_t, std::size_t> m_index;
    std::vector<system_data> m_data;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::ecs

#endif
