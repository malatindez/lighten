#pragma once
#include "render/model.hpp"
#include "render/shader-program.hpp"
#include "entt/entt.hpp"
namespace engine::render
{
    class ModelSystem;
    struct OpaqueMaterial
    {
        ID3D11ShaderResourceView *albedo;
        OpaqueMaterial() = default;
        explicit OpaqueMaterial(Material const &material) : albedo(material.texture) {}
        explicit OpaqueMaterial(ID3D11ShaderResourceView *albedo) : albedo(albedo) {}
    };
}
namespace std {
    template <>
    struct hash<engine::render::OpaqueMaterial>
    {
        std::size_t operator()(engine::render::OpaqueMaterial const &material) const
        {
            return std::hash<size_t>()(reinterpret_cast<size_t>(material.albedo));
        }
    };
}
namespace engine::components
{
    struct OpaqueComponent
    {
        uint64_t model_id;
    };
}
namespace engine::render::_opaque_detail
{
    struct OpaqueInstance
    {
        core::math::mat4 world_transform;
    };
    struct MaterialInstance
    {
        OpaqueMaterial material;
        std::set<entt::entity> instances;
    };
    struct MeshInstance
    {
        // Key is the hash of OpaqueMaterial
        std::unordered_map<size_t, MaterialInstance> material_instances;
    };
    struct ModelInstance
    {
        Model &model;
        // Each MeshInstance should correspond the one on the same index in model
        std::vector<MeshInstance> mesh_instances;
    };

    auto constexpr opaque_shader_path = "assets/shaders/opaque/opaque.hlsl";
    // This class can only be used as a member of ModelSystem
    class OpaqueRenderSystem
    {
    private:
        friend class ::engine::render::ModelSystem;

        OpaqueRenderSystem();
        void Render();

        void OnInstancesUpdated(entt::registry &registry);
        ModelInstance &GetInstance(uint64_t model_id);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<OpaqueMaterial> const &materials);
        // The key is model_id in model_system
        // Used to properly store IDs after their deletion
        // Deleted IDs will not be used and will be marked as tombstone ones
        std::unordered_map<uint64_t, ModelInstance> model_instances_;
        uint64_t current_index = 0;

        GraphicsShaderProgram opaque_shader_;
        direct3d::DynamicUniformBuffer<core::math::mat4> mesh_to_model_buffer_;
        direct3d::DynamicVertexBuffer<OpaqueInstance> instance_buffer_;
    };
} // namespace engine::render::_opaque_detail