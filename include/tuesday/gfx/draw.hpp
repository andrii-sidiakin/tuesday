#ifndef _TUE_GFX_DRAW_HPP_INCLUDED_
#define _TUE_GFX_DRAW_HPP_INCLUDED_

#include <utility>

namespace tue::gfx {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace details {

namespace adl_only {
void tue_init() = delete;
}

struct init_op {

    template <class Context> constexpr void operator()(Context &ctx) const {
        using namespace adl_only;
        tue_init(ctx);
    }

    template <class Context, class Obj>
    constexpr void operator()(Context &ctx, Obj &&obj) const {
        using namespace adl_only;
        tue_init(ctx, std::forward<Obj>(obj));
    }
};

constexpr init_op init{};

} // namespace details

using details::init;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace details {

namespace adl_only {
void tue_draw() = delete;
}

struct draw_op {

    template <class Context> constexpr void operator()(Context &ctx) const {
        using namespace adl_only;
        tue_draw(ctx);
    }

    template <class Context, class Obj>
    constexpr void operator()(Context &ctx, Obj &&obj) const {
        using namespace adl_only;
        tue_draw(ctx, std::forward<Obj>(obj));
    }
};

constexpr draw_op draw{};

} // namespace details

using details::draw;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace details {

namespace adl_only {
void tue_sync() = delete;
}

struct sync_op {

    template <class Context> constexpr void operator()(Context &ctx) const {
        using namespace adl_only;
        tue_sync(ctx);
    }

    template <class Context, class Obj>
    constexpr void operator()(Context &ctx, Obj &&obj) const {
        using namespace adl_only;
        tue_sync(ctx, std::forward<Obj>(obj));
    }
};

constexpr sync_op sync{};

} // namespace details

using details::sync;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::gfx

#endif
