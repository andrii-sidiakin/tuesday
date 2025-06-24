#include <tuesday/assert.hpp>
#include <tuesday/mp.hpp>

#include <tuesday/ecs.hpp>

#include <bitset>
#include <cstdint>

#include <print>

template <class Tag, typename T> struct Scalar {
    using value_type = T;
    value_type value{};

    explicit(false) constexpr operator value_type() const noexcept {
        return value;
    }
};

template <class Tag, typename T, std::size_t N> struct Vector {
    using value_type = T;
    value_type value[N]{};

    constexpr value_type &operator[](std::size_t i) noexcept {
        return value[i];
    }
};

template <typename T, std::size_t N, class Lhs, class Rhs>
auto operator*(Vector<Lhs, T, N> v, Scalar<Rhs, T> s) {
    Lhs u;
    for (auto i{0U}; i < N; ++i) {
        u[i] = v[i] * s;
    }
    return u;
}

template <typename T, std::size_t N, class Lhs, class Rhs>
auto operator+(Vector<Lhs, T, N> v, Vector<Rhs, T, N> u) {
    Lhs w;
    for (auto i{0U}; i < N; ++i) {
        w[i] = v[i] + u[i];
    }
    return w;
}

struct DeltaTime : Scalar<DeltaTime, float> {};
struct Mass : Scalar<Mass, float> {};
struct Position : Vector<Position, float, 3> {};
struct Velocity : Vector<Velocity, float, 3> {};
struct AAExtent : Vector<AAExtent, float, 3> {};

using ComponentSet = tue::mp::tseq<Mass, Position, Velocity, AAExtent>;

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

struct EntityTraits {
    using bitset_type = std::bitset<ComponentSet::size()>;

    static constexpr auto index = ComponentSet::make_index();

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

struct PhysicsSystem : public tue::ecs::basic_system<PhysicsSystem, Entity> {

    tue::ecs::entity_registry<Entity, EntityTraits> &m_reg;

    explicit PhysicsSystem(tue::ecs::entity_registry<Entity, EntityTraits> &reg)
        : m_reg(reg) {}

    void update(DeltaTime dt) {
        auto &x = m_reg.use_component<Position>();
        auto &v = m_reg.use_component<Velocity>();

        std::println("update: {}", entities().size());
        for (auto e : entities()) {
            x[e] = x[e] + v[e] * dt;
        }
    }
};

template <> struct tue::ecs::system_feature_tseq<PhysicsSystem> {
    using type = mp::tseq<Position, Velocity>;
};

int main() {
    tue::ecs::entity_registry<Entity, EntityTraits> reg;

    reg.make_component<Mass>();
    auto &pos = reg.make_component<Position>();
    reg.make_component<Velocity>();

    auto &physics = reg.make_system<PhysicsSystem>(reg);

    reg.insert(Entity::make());
    reg.insert(Entity::make(), Mass{}, Position{});

    auto e = Entity::make();
    reg.insert(e, Position{}, Velocity{});

    reg.insert(Entity::make(), Velocity{}, Position{}, Mass{});
    reg.insert(Entity::make(), Mass{}, Position{}, Velocity{});
    reg.insert(Entity::make(), Mass{}, Position{}, AAExtent{});

    physics.update({1});

    reg.erase(e);
    physics.update({1});

    return 0;
}
