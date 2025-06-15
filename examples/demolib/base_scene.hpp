#pragma once

#include "helpers.hpp"

struct render_context;

class base_scene {
  public:
    base_scene() = default;

    virtual void reset() {}
    virtual void update(delta_time dt) {}
    virtual void render(render_context &ctx) {}
};
