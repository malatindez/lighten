#pragma once
#include "include/library-pch.hpp"
#include "core/math.hpp"
namespace engine::core
{
    class Scene;
}
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
    // Determines render order
    // 0x00000000 - 0x0000FFFF - Depth, Shadow etc.
    // 0x00010000 - 0x0001FFFF - Opaque
    // 0x00020000 - 0x0002FFFF - Masked
    // 0x00030000 - 0x0003FFFF - Transparent
    // 0x00040000 - 0x0004FFFF - Postprocess
    // 0x00050000 - 0x0005FFFF - Debug
    // 0x00060000 - 0x0006FFFF - UI
    struct RenderPass
    {
        virtual void OnRender(core::Scene *scene) = 0;
        const uint32_t kPriority = 0;
        explicit RenderPass(uint32_t priority) : kPriority(priority) {}
        RenderPass(const RenderPass &other) = default;
        RenderPass(RenderPass &&other) = default;
        RenderPass &operator=(const RenderPass &other) = default;
        RenderPass &operator=(RenderPass &&other) = default;
        virtual ~RenderPass() = default;
    };
}