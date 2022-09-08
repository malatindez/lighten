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
        std::vector<ModelInstance> model_instances_;

        GraphicsShaderProgram opaque_shader_;
        direct3d::DynamicUniformBuffer<OpaqueInstance> mesh_to_model_buffer_;
        direct3d::DynamicVertexBuffer<OpaqueInstance> instance_buffer_;
    };
} // namespace engine::render::_opaque_detail