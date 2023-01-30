#pragma once
#include "opaque-render-system.hpp"

namespace engine::core
{
    class Scene;
}
namespace engine::render
{
    class ModelSystem;
    namespace _light_detail
    {
        class LightRenderSystem;
    }
}
namespace engine::components
{
    struct DecalComponent
    {
        struct Decal
        {
            core::math::mat4 mesh_transform;
            core::math::mat4 model_to_decal;
            core::math::vec3 base_color;
            // rotation of 2d texture in radians
            float texture_angle;
            float roughness;
            float metalness;
            float transmittance;
            float ambient_occlusion;
        };
        // The entity can have multiple decals so we need to store them in a vector
        // as we can't have multiple components of the same type on the same entity
        std::vector<Decal> decals;
    };
}
namespace engine::render::_decal_detail
{
    struct DecalInstance
    {
        core::math::mat4 world_transform;
        core::math::mat4 inv_world_transform;
        core::math::vec3 half_size;
        core::math::vec3 base_color;
        float texture_angle;
        float roughness;
        float metalness;
        float transmittance;
        float ambient_occlusion;
        uint32_t entity_id;
    };
    auto constexpr decal_vs_shader_path = "assets/shaders/decal/decal-vs.hlsl";
    auto constexpr decal_ps_shader_path = "assets/shaders/decal/decal-ps.hlsl";

    struct MaterialInstance
    {
        ID3D11ShaderResourceView *normal_opacity_map;
        uint32_t instances_amount;
    };

    class DecalRenderSystem final : public RenderPass
    {
    public:
        DecalRenderSystem();
        void OnRender(core::Scene *scene, GBuffer const &buffer, ID3D11DepthStencilView *dsv);

        void ScheduleInstanceUpdate()
        {
            is_instance_update_scheduled_ = true;
        }

        ID3D11ShaderResourceView *normal_opacity_map;
    private:
        bool is_instance_update_scheduled_ = false;
        void UpdateInstances(entt::registry &registry);
        // TODO:
        void on_attach(entt::registry &, entt::entity) {}
        void on_destroy(entt::registry &, entt::entity) {}
        void on_update(entt::registry &, entt::entity) {}

        GraphicsShaderProgram decal_shader_;
        std::vector<MaterialInstance> material_instances_;
        direct3d::DynamicVertexBuffer<DecalInstance> instance_buffer_;
        size_t instance_buffer_size_ = 0;
    };
} // namespace engine::render::_decal_detail