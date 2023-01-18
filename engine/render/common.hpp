#pragma once
#include "include/library-pch.hpp"
#include "core/math.hpp"
namespace engine::core
{
    class Scene;
}
namespace engine::direct3d
{
    class RenderTarget;
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
    struct GPUShadowPointLight
    {
        core::math::vec3 color;
        float padding;
        core::math::vec3 position;
        float radius;
        std::array<core::math::mat4, 6> view_projection;
    };
    struct GPUShadowSpotLight
    {
        core::math::vec3 color;
        float radius;
        core::math::vec3 direction;
        float inner_cutoff;
        core::math::vec3 position;
        float outer_cutoff;
        core::math::mat4 view_projection;
    };
    struct GPUShadowDirectionalLight
    {
        core::math::vec3 color;
        float padding;
        core::math::vec3 direction;
        float solid_angle;
        core::math::mat4 view_projection;
    };

    struct GPUPointLight
    {
        core::math::vec3 color;
        float padding;
        core::math::vec3 position;
        float radius;
    };
    struct GPUSpotLight
    {
        core::math::vec3 color;
        float radius;
        core::math::vec3 direction;
        float inner_cutoff;
        core::math::vec3 position;
        float outer_cutoff;
    };
    struct GPUDirectionalLight
    {
        core::math::vec3 color;
        float padding;
        core::math::vec3 direction;
        float solid_angle;
    };
    // Determines render order
    // 0x00000000 - 0x0000FFFF - Depth, Shadow etc.
    // 0x00010000 - 0x0001FFFF - Opaque
    // 0x00020000 - 0x0002FFFF - Masked
    // 0x00030000 - 0x0003FFFF - Transparent
    // 0x00040000 - 0x0004FFFF - Postprocessing
    // 0x00050000 - 0x0005FFFF - Debug
    // 0x00060000 - 0x0006FFFF - UI

    struct RenderPass
    {
        const uint32_t kPriority = 0;
        explicit RenderPass(uint32_t priority) : kPriority(priority) {}
        RenderPass(const RenderPass &other) = delete;
        RenderPass &operator=(const RenderPass &other) = delete;
        RenderPass(RenderPass &&other) = delete;
        RenderPass &operator=(RenderPass &&other) = delete;
        virtual ~RenderPass() = default;
    };

    struct GBuffer
    {
        std::shared_ptr<direct3d::RenderTarget> albedo;
        std::shared_ptr<direct3d::RenderTarget> normals;
        std::shared_ptr<direct3d::RenderTarget> roughness_metalness_transmittance_ao;
        std::shared_ptr<direct3d::RenderTarget> emission;
        std::shared_ptr<direct3d::RenderTarget> entity_id;
    };
}