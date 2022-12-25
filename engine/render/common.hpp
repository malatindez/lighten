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
        float time_now;
        float time_since_last_frame;
        uint32_t sample_count;
        float padding0;
    };
    struct GPUPointLight
    {
        core::math::vec3 color;
        float padding;
        core::math::vec3 position;
        float radius;
        std::array<core::math::mat4, 6> view_projection;
    };
    struct GPUSpotLight
    {
        core::math::vec3 color;
        float radius;
        core::math::vec3 direction;
        float inner_cutoff;
        core::math::vec3 position;
        float outer_cutoff;
        core::math::mat4 view_projection;
    };
    struct GPUDirectionalLight
    {
        core::math::vec3 color;
        float padding;
        core::math::vec3 direction;
        float solid_angle;
        core::math::mat4 view_projection;
    };
}