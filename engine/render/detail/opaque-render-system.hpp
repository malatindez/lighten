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
    auto constexpr opaque_vertex_shader_path = "assets/shaders/opaque/vs.hlsl";
    auto constexpr opaque_pixel_shader_path = "assets/shaders/opaque/ps.hlsl";
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
            uint64_t instance_id = 0;
            uint32_t amount = 0;

            [[nodiscard]] constexpr uint32_t mesh_id() const noexcept { return uint32_t(instance_id >> 32); }
            [[nodiscard]] constexpr uint32_t model_id() const noexcept { return uint32_t(instance_id & 0xffffffff); }

            InstanceInfo(Model &model, Mesh &mesh, Material &material, size_t instance_id) : model(model), mesh(mesh), material(material), instance_id(instance_id) {}
        };

        std::vector<InstanceInfo> instances_;

        GraphicsShaderProgram opaque_shader_;
        direct3d::DynamicUniformBuffer<core::math::mat4> mesh_to_model_buffer_;
        direct3d::DynamicVertexBuffer<Instance> instance_buffer_;
    };
} // namespace engine::render::_detail