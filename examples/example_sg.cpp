#include <tuesday/assert.hpp>
#include <tuesday/sg/scene_context.hpp>
#include <tuesday/sg/scene_node.hpp>

#include <map>
#include <memory>
#include <print>
#include <vector>

namespace tue::sg {

using entity_id_t = std::uint32_t;

struct entity {
    entity_id_t eid{};
};

struct camera {};
struct light {};
struct rigid_body {};

struct fetch_cmd {};
struct flush_cmd {};

using feature_set = tue::mp::tseq<entity, camera, light, rigid_body>;
using command_set = tue::mp::tseq<fetch_cmd, flush_cmd>;

template <class... Fs> struct features_map_key<mp::tseq<entity, Fs...>> {
    using type = entity_id_t;
};

///
///
class scene_context : tue::sg::basic_scene_context<feature_set, command_set> {
  public:
    static entity_id_t gen_eid() noexcept {
        static entity_id_t eid{0};
        return ++eid;
    }

    struct setup_cmd {
        scene_context &ctx;

        template <class F> constexpr void operator()(object &obj, F *fp) const {
            ctx.setup_feature(obj, fp);
        }
    };

  protected:
    friend struct setup_cmd;

    template <class F> void setup_feature(object &obj, F *fp) {
        const entity &e = obj.entity();

        if (e.eid == 0) {
            register_object(obj);
        }

        if (fp == nullptr) {
            allocate_feature(obj, mp::meta_for<F>);
        }

        std::println("reg-feature o={}, e={}, f={}, fp={}",
                     (void *)std::addressof(obj), e.eid, typeid(F).name(),
                     (void *)fp);
    }

  private:
    void register_object(object &obj) {
        auto &e = obj.entity();
        tue_assert(e.eid == 0, "Alredy registered");
        e.eid = gen_eid();
    }

    template <class F>
    constexpr void allocate_feature(object &obj, mp::meta<F> /*mt*/ = {}) {
        const auto &e = obj.entity();
        tue_assert(e.eid != 0, "Not registered");
        m_maps.insert_or_assign(e.eid, F{});
    }

  private:
    features_map<feature_set> m_maps;
};

///
///
class scene_node : tue::sg::basic_scene_node<feature_set, command_set> {
  public:
    scene_node() = default;
    scene_node(scene_node &&) noexcept = default;
    scene_node &operator=(scene_node &&) noexcept = default;

    template <typename T>
        requires(not std::same_as<T, scene_node>)
    explicit scene_node(T &&value)
        : m_obj{std::make_unique<object_impl<T>>(std::forward<T>(value))} {}

  public:
    template <typename T> void emplace_back(T &&value) {
        m_nodes.emplace_back(std::forward<T>(value));
    }

    template <typename T> void emplace_back(scene_node &&node) {
        m_nodes.emplace_back(std::move(node));
    }

  public:
    template <class Cmd> void apply(Cmd &&cmd) {
        if (m_obj) {
            m_obj->apply(std::forward<Cmd>(cmd));
        }
        for (auto &n : m_nodes) {
            n.apply(cmd);
        }
    }

  private:
    std::unique_ptr<object> m_obj;
    std::vector<scene_node> m_nodes;
};

} // namespace tue::sg

struct SceneCamera {};
struct SceneLight {};
struct SphericalTank {};
struct ParticleSystem {};

template <>
struct tue::sg::object_feature_traits<SceneCamera, tue::sg::feature_set> {
    using feature_tseq = tue::mp::tseq<camera>;
};

template <>
struct tue::sg::object_feature_traits<SceneLight, tue::sg::feature_set> {
    using feature_tseq = tue::mp::tseq<light>;
};

template <>
struct tue::sg::object_feature_traits<SphericalTank, tue::sg::feature_set> {
    using feature_tseq = tue::mp::tseq<rigid_body>;
};

template <>
struct tue::sg::object_feature_traits<ParticleSystem, tue::sg::feature_set> {
    using feature_tseq = tue::mp::tseq<light, rigid_body>;
};

template <class... Fs> struct overload : Fs... {
    using Fs::operator()...;
};

int main() {
    tue::sg::scene_node objs{SphericalTank{}};
    objs.emplace_back(ParticleSystem{});

    tue::sg::scene_node root;
    root.emplace_back(SceneCamera{});
    root.emplace_back(SceneLight{});
    root.emplace_back(std::move(objs));

    tue::sg::scene_context ctx;
    tue::sg::scene_context::setup_cmd setup_cmd{ctx};

    std::println("do setup");
    root.apply(setup_cmd);

    std::println("do setup (again)");
    root.apply(setup_cmd);

    std::println("do custom");
    root.apply(overload{
        [](auto &o, tue::sg::camera *f) {
            std::println("custom: e:{}, camera={}", o.entity().eid, (void *)f);
        },
        [](auto &o, tue::sg::light *f) {
            std::println("custom: e:{}, light={}", o.entity().eid, (void *)f);
        },
        [](auto &o, tue::sg::rigid_body *f) {
            std::println("custom: e:{}, rigid={}", o.entity().eid, (void *)f);
        },
        [](auto &o, auto *f) {
            std::println("custom: e:{}, f={}", o.entity().eid, (void *)f);
        },
    });

    return 0;
}
