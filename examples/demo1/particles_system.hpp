#pragma once

#include <tuesday/ecs.hpp>

#include <helpers.hpp>

#include <bitset>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class Tag, typename T> struct Value {
    using value_type = T;
    value_type value{};

    Value() = default;
    Value(const Value &) = default;
    Value &operator=(const Value &) = default;
    Value(Value &&) noexcept = default;
    Value &operator=(Value &&) noexcept = default;

    Value(const value_type &v) noexcept : value{v} {}
    Value(value_type &&v) noexcept : value{std::move(v)} {}

    explicit(false) constexpr operator value_type() const noexcept {
        return value;
    }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct Position : Value<Position, glm::vec3> {
    using Value::Value;
};
struct Velocity : Value<Velocity, glm::vec3> {
    using Value::Value;
};
struct Force : Value<Force, glm::vec3> {
    using Value::Value;
};
struct Color : Value<Force, glm::u8vec3> {
    using Value::Value;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct Entity {
    std::uint32_t id{0};

    friend constexpr bool operator==(Entity a, Entity b) noexcept {
        return a.id == b.id;
    }

    friend constexpr bool operator<(Entity a, Entity b) noexcept {
        return a.id < b.id;
    }

    static constexpr Entity make() noexcept {
        static std::uint32_t id{0};
        return Entity{++id};
    }
};

template <> struct std::hash<Entity> {
    auto operator()(const Entity &e) const noexcept {
        return std::hash<decltype(Entity::id)>{}(e.id);
    }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

using AllComponents = tue::mp::tseq<Position, Velocity, Force, Color>;

struct EntityTraits {
    using bitset_type = std::bitset<AllComponents::size()>;

    static constexpr auto index = AllComponents::make_index();

    constexpr void set(tue::mp::meta_index_t mt) {
        const auto *const iter = std::ranges::find(index, mt);
        if (iter != end(index)) {
            bits.set(std::distance(begin(index), iter), true);
        }
    }

    template <std::ranges::range Indices>
        requires(std::same_as<std::ranges::range_value_t<Indices>,
                              tue::mp::meta_index_t>)
    static constexpr auto make(Indices ids) {
        EntityTraits s{};
        for (auto id : ids) {
            s.set(id);
        }
        return s;
    }

    template <class... Cs> static consteval auto make(tue::mp::meta<Cs>... mt) {
        return make(std::array{mt.index()...});
    }

    template <class... Cs> static consteval auto make(tue::mp::tseq<Cs...> ts) {
        return make(ts.make_index());
    }

    friend constexpr bool operator==(EntityTraits a, EntityTraits b) noexcept {
        return a.bits == b.bits;
    }

    constexpr bool match(EntityTraits rhs) const noexcept {
        return bits == (bits & rhs.bits);
    }

    bitset_type bits;
};

using EntityRegistry = tue::ecs::entity_registry<Entity, EntityTraits>;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct PhysicsSystem : public tue::ecs::basic_system<PhysicsSystem, Entity> {

    EntityRegistry &m_reg;

    explicit PhysicsSystem(EntityRegistry &reg) : m_reg(reg) {}

    void update(float dt) {
        const glm::vec3 G = {0, -9.8, 0};

        auto &xs = m_reg.use_component<Position>();
        auto &vs = m_reg.use_component<Velocity>();
        auto &fs = m_reg.use_component<Force>();

        for (auto e : entities()) {
            auto mi = 1.F;
            auto a = fs[e].value + G * mi;
            auto v = vs[e].value + a * dt;
            auto x = xs[e].value + v * dt;
            xs[e] = x;
            vs[e] = v;
            fs[e] = {};
        }
    }
};

template <> struct tue::ecs::system_feature_tseq<PhysicsSystem> {
    using type = mp::tseq<Position, Velocity, Force>;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct CollisionSystem
    : public tue::ecs::basic_system<CollisionSystem, Entity> {

    EntityRegistry &m_reg;

    explicit CollisionSystem(EntityRegistry &reg) : m_reg(reg) {}

    void update(float dt) {
        auto &xs = m_reg.use_component<Position>();
        auto &vs = m_reg.use_component<Velocity>();

        for (auto e : entities()) {
            if (xs[e].value.y <= 0) {
                xs[e].value.y = 0;
                vs[e].value.y = -vs[e].value.y * 0.8;
            }
        }
    }
};

template <> struct tue::ecs::system_feature_tseq<CollisionSystem> {
    using type = mp::tseq<Position, Velocity>;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
