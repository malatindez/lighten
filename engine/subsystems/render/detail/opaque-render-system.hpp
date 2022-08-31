#pragma once
#include "render/shader-program.hpp"
#include "render/model.hpp"
#include "render/common.hpp"
namespace engine::components
{
    // Entity with MeshComponent and OpaqueComponent should be rendered using OpaqueRenderSystem.
    struct OpaqueComponent {};
}
namespace engine::render
{
    class ModelSystem;
}

namespace engine::render::_detail
{
    using ModelId = uint32_t;
    using MeshId = uint32_t;
    using MaterialId = uint32_t;
    struct InstanceId
    {
        ModelId model_id;
        MeshId mesh_id;
        MaterialId material_id;
        bool operator==(InstanceId const &other) const
        {
            return (model_id == other.model_id &&
                    mesh_id == other.mesh_id &&
                    material_id == other.material_id);
        }
    };
}
namespace engine::render::_detail
{
    auto constexpr opaque_shader_path = "assets/shaders/opaque/opaque.hlsl";
    // This class can only be used as a member of ModelSystemw
    class OpaqueRenderSystem
    {
    private:
        friend class ::engine::render::ModelSystem;

        struct Instance
        {
            core::math::mat4 model_transform;
        };

        OpaqueRenderSystem();
        void Render();

        void OnInstancesUpdated(entt::registry &registry);

        struct InstanceInfo
        {
            std::reference_wrapper<Model> model;
            std::reference_wrapper<Mesh> mesh;
            std::reference_wrapper<Material> material;
            // first 32 bits represent mesh_id
            // the next 32 bits represent model_id
            InstanceId kInstanceId;
            uint32_t amount = 0;

            InstanceInfo(Model &model, Mesh &mesh, Material &material, InstanceId kInstanceId, uint32_t amount) : model(model), mesh(mesh), material(material), kInstanceId(kInstanceId), amount(amount) {}
        };

        std::vector<InstanceInfo> instances_;

        GraphicsShaderProgram opaque_shader_;
        direct3d::DynamicUniformBuffer<core::math::mat4> mesh_to_model_buffer_;
        direct3d::DynamicVertexBuffer<Instance> instance_buffer_;
    };
} // namespace engine::render::_detail