#ifndef _TUE_ECS_REGISTRY_HPP_INCLUDED_
#define _TUE_ECS_REGISTRY_HPP_INCLUDED_

#include <tuesday/ecs/component.hpp>
#include <tuesday/ecs/system.hpp>

namespace tue::ecs {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// tuesday.ecs.registry

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
///
template <class Entity> class registry_base {
  public:
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

///
///
template <class Entity, class State>
class entity_registry : public registry_base<Entity> {
  public:
    using entity_type = Entity;
    using state_type = State;

    template <class S> using component = component_storage<Entity, S>;

  public:
    constexpr entity_registry() = default;
    constexpr entity_registry(entity_registry &&) noexcept = default;
    constexpr entity_registry &operator=(entity_registry &&) noexcept = default;

  public:
    template <class C> constexpr component<C> *find_component() const noexcept {
        return m_components.template find<C>();
    }

    template <class C, typename... Args>
    component<C> &make_component(Args &&...args) {
        return m_components.template make<C>(std::forward<Args>(args)...);
    }

    template <class C> component<C> &use_component() {
        return m_components.template use<C>();
    }

  public:
    template <class S> constexpr S *find_system() const noexcept {
        return m_systems.template find<S>();
    }

    template <class S, typename... Args> S &make_system(Args &&...args) {
        return m_systems.template make<S>(std::forward<Args>(args)...);
    }

    template <class S> S &use_system() { return m_systems.template use<S>(); }

  public:
    auto &entities() noexcept { return m_entities; }
    const auto &entities() const noexcept { return m_entities; }

  public:
    constexpr auto insert(entity_type e) { return m_entities.insert({e, {}}); }

    template <class... Cs>
        requires(sizeof...(Cs) > 0)
    auto insert(entity_type e, Cs &&...cs) {
        constexpr auto state = state_type::make(mp::meta_for<Cs>...);

        auto [it, inserted] = m_entities.insert({e, state});
        if (!inserted) {
            return false;
        }

        m_components.insert(e, std::forward<Cs>(cs)...);
        m_systems.insert(e, state);

        return true;
    }

    void erase(entity_type e) {
        auto it = m_entities.find(e);
        if (it != end(m_entities)) {
            auto state = it->second;
            m_entities.erase(it);
            m_components.erase(e);
            m_systems.erase(e, state);
        }
    }

  private:
    std::map<entity_type, state_type> m_entities;
    component_registry<entity_type> m_components;
    system_registry<entity_type, state_type> m_systems;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::ecs

#endif
