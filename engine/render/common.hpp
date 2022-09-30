#pragma once
#include "include/library-pch.hpp"
#include "core/math.hpp"
namespace engine::render
{
    struct PerFrame
    {
        core::math::mat4 view;
        core::math::mat4 projection;
        core::math::mat4 view_projection;
        core::math::mat4 inv_view;
        core::math::mat4 inv_projection;
        core::math::mat4 inv_view_projection;
        core::math::vec2 screen_resolution;
        core::math::vec2 mouse_position;
    };
}