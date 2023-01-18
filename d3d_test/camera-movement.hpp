#pragma once
#include "core/engine.hpp"
#include "render/render.hpp"
namespace camera_movement
{
    extern bool dynamic_shadows;
    [[nodiscard]] inline engine::core::math::ivec2 const &mouse_position() { return engine::core::InputLayer::instance()->mouse_position(); }

    void RegisterKeyCallbacks();

    void OnTick(float delta_time);

    void OnUpdate();
}