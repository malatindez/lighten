#pragma once
#include "core/engine.hpp"
#include "render/render.hpp"
namespace camera_movement
{
    [[nodiscard]] inline engine::core::math::ivec2 const &mouse_position() { return engine::core::InputLayer::instance()->mouse_position(); }

    void RegisterKeyCallbacks();

    void UpdateCamera(float delta_time);
}