#pragma once

#include "helpers.hpp"

struct render_context;

class base_scene {
  public:
    base_scene() = default;

    virtual void reset() {}
    virtual void update([[maybe_unused]] delta_time dt) {}
    virtual void render([[maybe_unused]] render_context &ctx) {}
};
