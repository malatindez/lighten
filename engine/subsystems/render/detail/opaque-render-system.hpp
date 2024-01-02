#pragma once
#include "render/model.hpp"
#include "render/shader-program.hpp"
#include "components/components.hpp"
#include "render/common.hpp"
namespace lighten::core
{
    class Scene;
}
namespace lighten::render
{
    class ModelSystem;
    namespace _light_detail
    {
        class LightRenderSystem;
    }
    namespace _opaque_detail
    {
        struct OpaquePerMaterial;
        class OpaqueRenderSystem;
    }
    struct OpaqueMaterial
    {
        ID3D11ShaderResourceView *albedo_map = nullptr;
        ID3D11ShaderResourceView *normal_map = nullptr;
        ID3D11ShaderResourceView *metalness_map = nullptr;
        ID3D11ShaderResourceView *roughness_map = nullptr;
        ID3D11ShaderResourceView *sheen_map = nullptr;

        // Used only to discard pixels with opacity < 0.5
        ID3D11ShaderResourceView *opacity_map = nullptr;
        glm::vec3 albedo_color{0.0f};
        float metalness_value = 0.0f;
        float roughness_value = 0.0f;
        glm::vec3 sheen_color{0.0f};
        float sheen_roughness = 0.0f;
        uint32_t texture_flags;
        bool reverse_normal_y = false;
        bool twosided = true;
        glm::vec2 uv_multiplier{1};
        void UpdateTextureFlags();
        OpaqueMaterial() { reset(); }
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
            albedo_color = glm::vec3{0.0f};
            metalness_value = 0.01f;
            roughness_value = 0.01f;
            sheen_color = glm::vec3{0.0f};
            sheen_roughness = 0.00f;
            texture_flags = 0;
            reverse_normal_y = false;
            twosided = true;
            uv_multiplier = glm::vec2{1};
        }
    };
}
namespace std
{
    template <>
    struct hash<lighten::render::OpaqueMaterial>
    {
        std::size_t operator()(lighten::render::OpaqueMaterial const &material) const
        {
            size_t seed = 0;
            mal_toolkit::hash_combine(seed, material.albedo_map);
            mal_toolkit::hash_combine(seed, material.normal_map);
            mal_toolkit::hash_combine(seed, material.metalness_map);
            mal_toolkit::hash_combine(seed, material.roughness_map);
            mal_toolkit::hash_combine(seed, material.albedo_color);
            mal_toolkit::hash_combine(seed, material.metalness_value);
            mal_toolkit::hash_combine(seed, material.roughness_value);
            mal_toolkit::hash_combine(seed, material.texture_flags);
            mal_toolkit::hash_combine(seed, material.reverse_normal_y);
            mal_toolkit::hash_combine(seed, material.uv_multiplier);
            return seed;
        }
    };
}
namespace lighten::components
{
    struct OpaqueComponent
    {
    public:
        OpaqueComponent(size_t model_id) : model_id(model_id) {}
        size_t model_id;

    private:
        friend class ::lighten::render::_opaque_detail::OpaqueRenderSystem;
        size_t model_instance_id;
        std::vector<size_t> material_instance_id;
    };
}
namespace lighten::render::_opaque_detail
{
    struct OpaqueInstance
    {
        glm::mat4 world_transform;
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
        glm::vec3 albedo_color;
        float metalness;
        float roughness;
        glm::vec3 sheen_color;
        float sheen;
        uint32_t enabled_texture_flags;
        glm::vec2 uv_multiplier;
    };
    struct OpaquePerDepthCubemap
    {
        std::array<glm::mat4, 6> g_view_projection;
        uint32_t g_slice_offset;
        glm::vec3 padding0;
    };
    struct OpaquePerDepthTexture
    {
        glm::mat4 g_view_projection;
        uint32_t g_slice_offset;
        glm::vec3 padding0;
    };
    struct OpaqueInstanceInfo
    {
        uint64_t model_id;
        std::vector<OpaqueMaterial const *> materials;
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

        /** @brief Returns instance information
         * @warning pointers to materials aren't guaranteed to point to the correct location after UpdateInstances was called
         */
        OpaqueInstanceInfo GetInstanceInfo(entt::registry &registry, entt::entity entity)
        {
            OpaqueInstanceInfo rv;
            components::OpaqueComponent &oc = registry.get<components::OpaqueComponent>(entity);
            rv.model_id = oc.model_id;
            ModelInstance &model_instance = model_instances_.at(oc.model_instance_id);
            auto &materials = rv.materials;
            uint32_t i = 0;
            for (auto material_instance_id : oc.material_instance_id)
            {
                materials.emplace_back(&model_instance.mesh_instances[i++].material_instances[material_instance_id].material);
            }
            return rv;
        }
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<OpaqueMaterial> const &materials);
        void RemoveInstance(entt::registry &registry, entt::entity entity);

        void Update([[maybe_unused]] core::Scene *scene) {}
        void ScheduleInstanceUpdate()
        {
            is_instance_update_scheduled_ = true;
        }

    private:
        size_t GetInstanceId(uint64_t model_id);

        void UpdateInstances(entt::registry &registry);

        // TODO:
        void on_attach(entt::registry &, entt::entity) {}
        void on_destroy(entt::registry &, entt::entity) {}

        bool is_instance_update_scheduled_ = false;

        std::unordered_map<size_t, ModelInstance> model_instances_;
        size_t model_instance_offset_ = 0;

        GraphicsShaderProgram opaque_cubemap_shader_;
        GraphicsShaderProgram opaque_texture_shader_;
        direct3d::DynamicUniformBuffer<OpaquePerDepthCubemap> opaque_per_cubemap_buffer_;
        direct3d::DynamicUniformBuffer<OpaquePerDepthTexture> opaque_per_texture_buffer_;

        GraphicsShaderProgram opaque_shader_;
        direct3d::DynamicUniformBuffer<OpaquePerMaterial> opaque_per_material_buffer_;
        direct3d::DynamicUniformBuffer<glm::mat4> mesh_to_model_buffer_;
        direct3d::DynamicVertexBuffer<OpaqueInstance> instance_buffer_;
    };
} // namespace lighten::render::_opaque_detail