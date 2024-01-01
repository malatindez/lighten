#pragma once
#include "render/model.hpp"
#include "render/shader-program.hpp"
#include "render/shadow-map.hpp"
#include "core/math.hpp"
#include "components/components.hpp"
#include "render/common.hpp"
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

        struct LightsPerFrame
        {
            static constexpr uint32_t kMaxShadowPointLights = 8;
            static constexpr uint32_t kMaxShadowSpotLights = 4;
            static constexpr uint32_t kMaxShadowDirectionalLights = 2;
            static constexpr uint32_t kMaxPointLights = 32;
            static constexpr uint32_t kMaxSpotLights = 32;
            static constexpr uint32_t kMaxDirectionalLights = 4;

            std::array<GPUShadowPointLight, kMaxShadowPointLights> shadow_point_lights;
            std::array<GPUShadowSpotLight, kMaxShadowSpotLights> shadow_spot_lights;
            std::array<GPUShadowDirectionalLight, kMaxShadowDirectionalLights> shadow_directional_lights;
            std::array<GPUPointLight, kMaxPointLights> point_lights;
            std::array<GPUSpotLight, kMaxSpotLights> spot_lights;
            std::array<GPUDirectionalLight, kMaxDirectionalLights> directional_lights;
            uint32_t shadow_num_point_lights;
            uint32_t shadow_num_spot_lights;
            uint32_t shadow_num_directional_lights;

            uint32_t num_point_lights;
            uint32_t num_spot_lights;
            uint32_t num_directional_lights;

            uint32_t point_light_shadow_resolution;
            uint32_t spot_light_shadow_resolution;
            uint32_t directional_light_shadow_resolution;

            glm::vec3 g_lights_per_frame_padding_0;
        };
    }
}
namespace engine::render::_light_detail
{
    class LightRenderSystem final : public RenderPass
    {
    public:
        void ResizeShadowMaps(uint32_t resolution) { resolution_ = resolution; }
        [[nodiscard]] uint32_t point_light_shadow_resolution() const noexcept { return resolution_; }
        [[nodiscard]] uint32_t spot_light_shadow_resolution() const noexcept { return resolution_; }
        [[nodiscard]] uint32_t directional_light_shadow_resolution() const noexcept { return resolution_; }
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
        [[nodiscard]] int32_t get_point_light_index(entt::entity entity) const noexcept
        {
            if (point_light_shadow_map_indices_.contains(entity))
            {
                return point_light_shadow_map_indices_.at(entity);
            }
            return -1;
        }
        [[nodiscard]] int32_t get_spot_light_index(entt::entity entity) const noexcept
        {
            if (spot_light_shadow_map_indices_.contains(entity))
            {
                return spot_light_shadow_map_indices_.at(entity);
            }
            return -1;
        }
        [[nodiscard]] int32_t get_directional_light_index(entt::entity entity) const noexcept
        {
            if (directional_light_shadow_map_indices_.contains(entity))
            {
                return directional_light_shadow_map_indices_.at(entity);
            }
            return -1;
        }
        void BindPointShadowMaps(int slot) { point_light_shadow_maps_.BindShaderResource(slot); }
        void BindSpotShadowMaps(int slot) { spot_light_shadow_maps_.BindShaderResource(slot); }
        void BindDirectionalShadowMaps(int slot) { directional_light_shadow_maps_.BindShaderResource(slot); }
        LightRenderSystem() : RenderPass(0) {}
        virtual ~LightRenderSystem() = default;
        LightRenderSystem(LightRenderSystem const &) = delete;
        LightRenderSystem(LightRenderSystem &&) = delete;
        LightRenderSystem &operator=(LightRenderSystem const &) = delete;
        LightRenderSystem &operator=(LightRenderSystem &&) = delete;

        void OnRender(core::Scene *scene);
        void Update([[maybe_unused]] core::Scene *scene) {}
        // Will update shadow maps if the last update was more than shadow_map_update_interval_ seconds ago
        void ScheduleShadowMapUpdate()
        {
            should_update = true;
        }
        void ScheduleInstanceUpdate()
        {
            is_instance_update_scheduled_ = true;
        }

    private:
        void UpdateInstances(core::Scene *scene);
        bool is_instance_update_scheduled_ = false;

        bool should_update = false;
        bool refresh_data = false;
        mal_toolkit::SteadyTimer shadow_map_update_timer_;
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
        std::unordered_map<entt::entity, std::array<glm::mat4, 6>> point_light_shadow_matrices_;
        std::unordered_map<entt::entity, glm::mat4> spot_light_shadow_matrices_;
        std::unordered_map<entt::entity, glm::mat4> directional_light_shadow_matrices_;
        std::unordered_map<entt::entity, uint32_t> point_light_shadow_map_indices_;
        std::unordered_map<entt::entity, uint32_t> spot_light_shadow_map_indices_;
        std::unordered_map<entt::entity, uint32_t> directional_light_shadow_map_indices_;
        GraphicsShaderProgram shadowmap_shader_;
        LightsPerFrame per_frame;
        direct3d::DynamicUniformBuffer<LightsPerFrame> lights_per_frame_;
        uint32_t resolution_ = 2048;
    };
} // namespace engine::render::_light_detail