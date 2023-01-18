#pragma once
#include "render/model.hpp"
#include "render/shader-program.hpp"
#include "entt/entt.hpp"
#include "components/components.hpp"
#include "render/common.hpp"
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
    namespace _opaque_detail
    {
        struct OpaquePerMaterial;
    }
    struct OpaqueMaterial
    {
        ID3D11ShaderResourceView *albedo_map = nullptr;
        ID3D11ShaderResourceView *normal_map = nullptr;
        ID3D11ShaderResourceView *metalness_map = nullptr;
        ID3D11ShaderResourceView *roughness_map = nullptr;

        // Used only to discard pixels with opacity < 0.5
        ID3D11ShaderResourceView *opacity_map = nullptr;
        core::math::vec3 albedo_color;
        float metalness_value;
        float roughness_value;
        uint32_t texture_flags;
        bool reverse_normal_y = false;
        bool twosided = false;
        core::math::vec2 uv_multiplier{ 1 };
        void UpdateTextureFlags();
        OpaqueMaterial() = default;
        void BindTextures() const;
        void Bind(direct3d::DynamicUniformBuffer<_opaque_detail::OpaquePerMaterial> &uniform_buffer) const;
        explicit OpaqueMaterial(Material const &material);
        void reset()
        {
            albedo_map = nullptr;
            normal_map = nullptr;
            metalness_map = nullptr;
            roughness_map = nullptr;
            opacity_map = nullptr;
            albedo_color = core::math::vec3{ 0.0f };
            metalness_value = 0.01f;
            roughness_value = 0.01f;
            texture_flags = 0;
            reverse_normal_y = false;
            twosided = false;
            uv_multiplier = core::math::vec2{ 1 };
        }
    };
}
namespace std {
    template <>
    struct hash<engine::render::OpaqueMaterial>
    {
        std::size_t operator()(engine::render::OpaqueMaterial const &material) const
        {
            size_t seed = 0;
            engine::utils::hash_combine(seed, material.albedo_map);
            engine::utils::hash_combine(seed, material.normal_map);
            engine::utils::hash_combine(seed, material.metalness_map);
            engine::utils::hash_combine(seed, material.roughness_map);
            engine::utils::hash_combine(seed, material.albedo_color);
            engine::utils::hash_combine(seed, material.metalness_value);
            engine::utils::hash_combine(seed, material.roughness_value);
            engine::utils::hash_combine(seed, material.texture_flags);
            engine::utils::hash_combine(seed, material.reverse_normal_y);
            engine::utils::hash_combine(seed, material.uv_multiplier);
            return seed;
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
        uint32_t entity_id;
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
    struct OpaquePerMaterial
    {
        core::math::vec3 albedo_color;
        float metalness;
        float roughness;
        uint32_t enabled_texture_flags;
        core::math::vec2 uv_multiplier;
    };
    struct OpaquePerDepthCubemap
    {
        std::array<core::math::mat4, 6> g_view_projection;
        uint32_t g_slice_offset;
        core::math::vec3 padding0;
    };
    struct OpaquePerDepthTexture
    {
        core::math::mat4 g_view_projection;
        uint32_t g_slice_offset;
        core::math::vec3 padding0;
    };

    auto constexpr opaque_vs_shader_path = "assets/shaders/opaque/opaque-vs.hlsl";
    auto constexpr opaque_ps_shader_path = "assets/shaders/opaque/opaque-ps.hlsl";
    auto constexpr opaque_vs_depth_only_shader_path = "assets/shaders/opaque/opaque-depth-only-vs.hlsl";
    auto constexpr opaque_gs_depth_only_cubemap_shader_path = "assets/shaders/opaque/opaque-depth-only-cubemap-gs.hlsl";
    auto constexpr opaque_gs_depth_only_texture_shader_path = "assets/shaders/opaque/opaque-depth-only-texture-gs.hlsl";
    auto constexpr opaque_ps_depth_only_shader_path = "assets/shaders/opaque/opaque-depth-only-ps.hlsl";

    // TODO:
    // update opaque, emissive and skybox rendering systems so we don't have to call AddInstance
    // We will use on_construct and on_destruct to change it
    // TODO:
    // We should store list of all available materials as an unordered map of opaque materials in opaque render system.
    // This way we can avoid creating new materials every time we add a new instance
    // TODO:
    // We should create OpaqueComponent and store the vector of size_t hashes of materials in it
    // and store model_id as well

    // We will probably need to have the private cache variable in it so we can delete instances of materials that aren't used

    // on_destruct / on_update / on_construct should update the instance tree using these materials
    // if hash differs -> update material, update cache, delete previous instance
    // if hash is the same -> do nothing

    // TODO:
    // Change the system the way that on_destruct / on_construct will set the dirty flag
    // and add data that should be updated so we can update instances right before the frame if needed

    class OpaqueRenderSystem final : public RenderPass
    {
    public:
        ModelInstance *GetInstancePtr(uint64_t model_id);
        OpaqueRenderSystem();
        void OnRender(core::Scene *scene);
        void RenderDepthOnly(std::vector<OpaquePerDepthCubemap> const &cubemaps, core::Scene *scene);
        void RenderDepthOnly(std::vector<OpaquePerDepthTexture> const &textures, core::Scene *scene);

        ModelInstance &GetInstance(uint64_t model_id);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<OpaqueMaterial> const &materials);

        void Update([[maybe_unused]] core::Scene *scene) {}
        void ScheduleInstanceUpdate()
        {
            is_instance_update_scheduled_ = true;
        }
    private:
        void UpdateInstances(entt::registry &registry);

        // TODO:
        void on_attach(entt::registry &, entt::entity) {}
        void on_destroy(entt::registry &, entt::entity) {}

        bool is_instance_update_scheduled_ = false;

        std::vector<ModelInstance> model_instances_;

        GraphicsShaderProgram opaque_cubemap_shader_;
        GraphicsShaderProgram opaque_texture_shader_;
        direct3d::DynamicUniformBuffer<OpaquePerDepthCubemap> opaque_per_cubemap_buffer_;
        direct3d::DynamicUniformBuffer<OpaquePerDepthTexture> opaque_per_texture_buffer_;

        GraphicsShaderProgram opaque_shader_;
        direct3d::DynamicUniformBuffer<OpaquePerMaterial> opaque_per_material_buffer_;
        direct3d::DynamicUniformBuffer<core::math::mat4> mesh_to_model_buffer_;
        direct3d::DynamicVertexBuffer<OpaqueInstance> instance_buffer_;
    };
} // namespace engine::render::_opaque_detail