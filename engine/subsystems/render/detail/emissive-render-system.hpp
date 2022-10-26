#pragma once
#include "render/model.hpp"
#include "render/shader-program.hpp"
#include "entt/entt.hpp"
namespace engine::render
{
    class ModelSystem;
    struct EmissiveMaterial
    {
        ID3D11ShaderResourceView *emissive_texture = nullptr;
        core::math::vec3 emissive_color;
        float power = 1;
        EmissiveMaterial() = default;
        explicit EmissiveMaterial(Material const &material, float power = 1)
        {
            emissive_texture = material.emissive_textures.size() > 0 ? material.emissive_textures.front() : nullptr;
            emissive_color = material.emissive_color;
            this->power = power;
        }
        explicit EmissiveMaterial(ID3D11ShaderResourceView *emissive_texture, float power = 1) : emissive_texture(emissive_texture), power(power) {}
        explicit EmissiveMaterial(core::math::vec3 const &emissive_color, float power = 1) : emissive_color(emissive_color), power(power) {}
    };
}
namespace std {
    template <>
    struct hash<engine::render::EmissiveMaterial>
    {
        std::size_t operator()(engine::render::EmissiveMaterial const &material) const
        {
            return std::hash<size_t>()(reinterpret_cast<size_t>(material.emissive_texture)) ^ std::hash<engine::core::math::vec3>()(material.emissive_color);
        }
    };
}
namespace engine::components
{
    struct EmissiveComponent
    {
        uint64_t model_id;
    };
}
namespace engine::render::_emissive_detail
{
    struct EmissiveInstance
    {
        core::math::mat4 world_transform;
        core::math::vec3 emissive_color;
    };
    struct EmissiveShaderBuffer
    {
        core::math::mat4 mesh_to_model;
        int use_emissive_texture = 0; // if != 0 then use emissive texture
        float power;
        core::math::vec2 padding;
    };
    struct MaterialInstance
    {
        EmissiveMaterial material;
        std::vector<entt::entity> instances;
    };
    struct MeshInstance
    {
        std::vector<MaterialInstance> material_instances;
    };
    struct ModelInstance
    {
        Model &model;
        uint64_t model_id;
        // Each MeshInstance should correspond the one on the same index in model
        std::vector<MeshInstance> mesh_instances;
    };

    // This class can only be used as a member of ModelSystem
    class EmissiveRenderSystem
    {
    public:
        ModelInstance *GetInstancePtr(uint64_t model_id);

        EmissiveRenderSystem();
        void Render();
        ModelInstance &GetInstance(uint64_t model_id);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<EmissiveMaterial> const &materials);

        void OnInstancesUpdated(entt::registry &registry);

    private:
        static auto constexpr emissive_vs_shader_path = "assets/shaders/emissive/emissive-vs.hlsl";
        static auto constexpr emissive_ps_shader_path = "assets/shaders/emissive/emissive-ps.hlsl";
        std::vector<ModelInstance> model_instances_;

        GraphicsShaderProgram emissive_shader_;
        direct3d::DynamicUniformBuffer<EmissiveShaderBuffer> emissive_shader_buffer_;
        direct3d::DynamicVertexBuffer<EmissiveInstance> instance_buffer_;
    };
} // namespace engine::render::_emissive_detail