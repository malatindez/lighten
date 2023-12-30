#pragma once
#include "render/model.hpp"
#include "render/shader-program.hpp"
#include "render/common.hpp"
namespace engine::core
{
    class Scene;
}
namespace engine::render
{
    class ModelSystem;
    namespace _emissive_detail
    {
        class EmissiveRenderSystem;
    }
}
namespace engine::components
{
    struct EmissiveComponent
    {
        uint64_t model_id;
        core::math::vec3 emissive_color;
        float power;
    };
} // namespace engine::components
namespace engine::render::_emissive_detail
{
    struct EmissiveShaderBuffer
    {
        core::math::mat4 mesh_to_model;
    };
    struct EmissiveInstance
    {
        core::math::mat4 world_transform;
        core::math::vec3 emissive_color;
    };
    struct MeshInstance
    {
        std::vector<entt::entity> instances;
    };
    struct ModelInstance
    {
        Model &model;
        uint64_t model_id;
        // Each MeshInstance should correspond the one on the same index in model
        std::vector<MeshInstance> mesh_instances;
    };

    // This class can only be used as a member of ModelSystem
    class EmissiveRenderSystem final : public render::RenderPass
    {
    public:
        ModelInstance *GetInstancePtr(uint64_t model_id);

        EmissiveRenderSystem();
        void OnRender(core::Scene *scene);

        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity);

        void Update([[maybe_unused]] core::Scene *scene) {}
        void ScheduleInstanceUpdate()
        {
            is_instance_update_scheduled_ = true;
        }

    private:
        ModelInstance &GetInstance(uint64_t model_id);
        void UpdateInstances(entt::registry &registry);

        static auto constexpr emissive_vs_shader_path = "assets/shaders/emissive/emissive-vs.hlsl";
        static auto constexpr emissive_ps_shader_path = "assets/shaders/emissive/emissive-ps.hlsl";

        bool is_instance_update_scheduled_ = false;

        std::vector<ModelInstance> model_instances_;

        GraphicsShaderProgram emissive_shader_;
        direct3d::DynamicUniformBuffer<EmissiveShaderBuffer> emissive_shader_buffer_;
        direct3d::DynamicVertexBuffer<EmissiveInstance> instance_buffer_;
    };
} // namespace engine::render::_emissive_detail