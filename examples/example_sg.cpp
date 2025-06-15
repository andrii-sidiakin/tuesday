#include "scene_context.hpp"

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
