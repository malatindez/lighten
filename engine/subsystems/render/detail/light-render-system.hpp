#pragma once
#include "render/model.hpp"
#include "render/shader-program.hpp"
#include "render/shadow-map.hpp"
#include "entt/entt.hpp"
#include "core/math.hpp"
#include "components/components.hpp"
namespace engine
{
    namespace core
    {
        class Scene;
    }
    namespace render
    {
        class ModelSystem;
        namespace _opaque_detail
        {
            struct OpaquePerDepthCubemap;
            struct OpaquePerDepthTexture;
        }
    }
}
namespace engine::render::_light_detail
{
    class LightRenderSystem
    {
    public:
        void ResizeShadowMaps(size_t resolution) { resolution_ = resolution; }
        [[nodiscard]] size_t point_light_shadow_resolution() const noexcept { return resolution_; }
        [[nodiscard]] size_t spot_light_shadow_resolution() const noexcept { return resolution_; }
        [[nodiscard]] size_t directional_light_shadow_resolution() const noexcept { return resolution_; }
        [[nodiscard]] auto const &point_light_entities() const noexcept { return point_light_entities_; }
        [[nodiscard]] auto const &spot_light_entities() const noexcept { return spot_light_entities_; }
        [[nodiscard]] auto const &directional_light_entities() const noexcept { return directional_light_entities_; }
        [[nodiscard]] auto const &point_light_shadow_matrices() const noexcept { return point_light_shadow_matrices_; }
        [[nodiscard]] auto const &spot_light_shadow_matrices() const noexcept { return spot_light_shadow_matrices_; }
        [[nodiscard]] auto const &directional_light_shadow_matrices() const noexcept { return directional_light_shadow_matrices_; }
        [[nodiscard]] auto const &shadow_map_update_interval() const noexcept { return shadow_map_update_interval_; }
        [[nodiscard]] auto &shadow_map_update_interval() noexcept { return shadow_map_update_interval_; }
        [[nodiscard]] auto const &point_light_shadow_maps() const noexcept { return point_light_shadow_maps_; }
        [[nodiscard]] auto const &spot_light_shadow_maps() const noexcept { return spot_light_shadow_maps_; }
        [[nodiscard]] auto const &directional_light_shadow_maps() const noexcept { return directional_light_shadow_maps_; }

        void BindPointShadowMaps(int slot) { point_light_shadow_maps_.BindShaderResource(slot); }
        void BindSpotShadowMaps(int slot) { spot_light_shadow_maps_.BindShaderResource(slot); }
        void BindDirectionalShadowMaps(int slot) { directional_light_shadow_maps_.BindShaderResource(slot); }
        LightRenderSystem() = default;
        ~LightRenderSystem() = default;
        LightRenderSystem(LightRenderSystem const &) = delete;
        LightRenderSystem(LightRenderSystem &&) = delete;
        LightRenderSystem &operator=(LightRenderSystem const &) = delete;
        LightRenderSystem &operator=(LightRenderSystem &&) = delete;

        void OnInstancesUpdated(core::Scene *scene);
        void RenderShadowMaps(core::Scene *scene);
        void Update(core::Scene *scene)
        {
            if (should_update && shadow_map_update_timer_.elapsed() > shadow_map_update_interval_)
            {
                shadow_map_update_timer_.reset();
                RenderShadowMaps(scene);
                should_update = false;
            }
        }
        // Will update shadow maps if the last update was more than shadow_map_update_interval_ seconds ago
        void ScheduleShadowMapUpdate()
        {
            should_update = true;
        }

    private:
        bool should_update = false;
        bool refresh_data = false;
        utils::SteadyTimer shadow_map_update_timer_;
        float last_update_;
        float shadow_map_update_interval_ = 0.0f;

        void ProcessPointLights(core::Scene *scene);
        void ProcessSpotLights(core::Scene *scene);
        void ProcessDirectionalLights(core::Scene *scene);
        ShadowMap point_light_shadow_maps_;
        ShadowMap spot_light_shadow_maps_;
        ShadowMap directional_light_shadow_maps_;
        std::vector<entt::entity> point_light_entities_;
        std::vector<entt::entity> spot_light_entities_;
        std::vector<entt::entity> directional_light_entities_;
        std::vector<entt::entity> point_light_entities_temp_;
        std::vector<entt::entity> spot_light_entities_temp_;
        std::vector<entt::entity> directional_light_entities_temp_;
        std::unordered_map<entt::entity, std::array<core::math::mat4, 6>> point_light_shadow_matrices_;
        std::unordered_map<entt::entity, core::math::mat4> spot_light_shadow_matrices_;
        std::unordered_map<entt::entity, core::math::mat4> directional_light_shadow_matrices_;
        GraphicsShaderProgram shadowmap_shader_;
        size_t resolution_ = 2048;
    };
} // namespace engine::render::_light_detail