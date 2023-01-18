#pragma once
#include "opaque-render-system.hpp"

// TODO (malatindez):
// Create generalized render system class
// Basically dissolution, opaque and emissive rendering systems just pass
// instances as the array of matrices and are pretty much similar in many ways
// We can create a generalized abstract render system class that will take instances
// as the array of matrices and will render them
// This class will be templated, and the GPUPerMaterial and the RegistryInstance typenames should be passed as template parameters

// GPUPerMaterial should have a constructor that takes render::Material as the parameter to initialize
// Also it should have the Bind function, so it will bind the needed textures and constants to the shader

// RegistryInstance will be used to bind on_construct, on_destroy and on_update functions
// as well as the model_id member variable
// RegistryInstance should have model_id to be able to retrieve the model to the shader

// Main shader as well as shadow shaders(if needed) should be passed as the parameters to the constructor
// Also the constructor should have the boolean parameter that will specify if shadows are enabled
// This way the code will be much simpler to read and maintain

// The descendant class should only implement the on_construct, on_destroy and on_update functions for RegistryInstance and TransformComponent
// It can overload OnRender function if needed
// RegistryInstance should have size_t model_id as a member variable

// For now, I will copy the code from opaque rendering system and will modify it to fit the new system
// so we can complete the project as fast as possible.
// After that, I will create the generalized class and will refactor the code
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
    namespace _dissolution_detail
    {
        struct DissolutionPerMaterial;
    }
    struct DissolutionMaterial
    {
        ID3D11ShaderResourceView *albedo_map = nullptr;
        ID3D11ShaderResourceView *normal_map = nullptr;
        ID3D11ShaderResourceView *metalness_map = nullptr;
        ID3D11ShaderResourceView *roughness_map = nullptr;
        ID3D11ShaderResourceView *opacity_map = nullptr;
        core::math::vec3 albedo_color = core::math::vec3{ 0.0f };
        float metalness_value = 0.01f;
        float roughness_value = 0.01f;
        uint32_t flags = 0;
        bool reverse_normal_y = false;
        bool twosided = false;
        bool appearing = true;
        bool emissive = true;
        core::math::vec2 uv_multiplier{ 1 };

        OpaqueMaterial ToOpaqueMaterial() const
        {
            OpaqueMaterial material;
            material.albedo_map = albedo_map;
            material.normal_map = normal_map;
            material.metalness_map = metalness_map;
            material.roughness_map = roughness_map;
            material.opacity_map = opacity_map;
            material.albedo_color = albedo_color;
            material.metalness_value = metalness_value;
            material.roughness_value = roughness_value;
            material.reverse_normal_y = reverse_normal_y;
            material.twosided = twosided;
            material.uv_multiplier = uv_multiplier;
            material.UpdateTextureFlags();
            return material;
        }
        static DissolutionMaterial FromOpaqueMaterial(OpaqueMaterial const &material) noexcept
        {
            DissolutionMaterial result;
            result.albedo_map = material.albedo_map;
            result.normal_map = material.normal_map;
            result.metalness_map = material.metalness_map;
            result.roughness_map = material.roughness_map;
            result.opacity_map = material.opacity_map;
            result.albedo_color = material.albedo_color;
            result.metalness_value = material.metalness_value;
            result.roughness_value = material.roughness_value;
            result.reverse_normal_y = material.reverse_normal_y;
            result.twosided = material.twosided;
            result.uv_multiplier = material.uv_multiplier;
            result.UpdateTextureFlags();
            return result;
        }
        void UpdateTextureFlags();
        DissolutionMaterial() = default;
        void BindTextures() const;
        void Bind(direct3d::DynamicUniformBuffer<_dissolution_detail::DissolutionPerMaterial> &uniform_buffer) const;
        explicit DissolutionMaterial(Material const &material);
        void reset()
        {
            *this = DissolutionMaterial{};
        }
    };
}
namespace std {
    template <>
    struct hash<engine::render::DissolutionMaterial>
    {
        std::size_t operator()(engine::render::DissolutionMaterial const &material) const
        {
            size_t seed = 0;
            engine::utils::hash_combine(seed, material.albedo_map);
            engine::utils::hash_combine(seed, material.normal_map);
            engine::utils::hash_combine(seed, material.metalness_map);
            engine::utils::hash_combine(seed, material.roughness_map);
            engine::utils::hash_combine(seed, material.albedo_color);
            engine::utils::hash_combine(seed, material.metalness_value);
            engine::utils::hash_combine(seed, material.roughness_value);
            engine::utils::hash_combine(seed, material.flags);
            engine::utils::hash_combine(seed, material.reverse_normal_y);
            engine::utils::hash_combine(seed, material.twosided);
            engine::utils::hash_combine(seed, material.appearing);
            engine::utils::hash_combine(seed, material.emissive);
            engine::utils::hash_combine(seed, material.uv_multiplier);
            return seed;
        }
    };
}
namespace engine::components
{
    struct DissolutionComponent
    {
        uint64_t model_id;
        float time_begin;
        float lifetime;
    };
}
namespace engine::render::_dissolution_detail
{
    struct DissolutionInstance
    {
        core::math::mat4 world_transform;
        float time_begin;
        float lifetime;
    };
    using DissolutionPerDepthCubemap = _opaque_detail::OpaquePerDepthCubemap;
    using DissolutionPerDepthTexture = _opaque_detail::OpaquePerDepthTexture;

    struct MaterialInstance
    {
        DissolutionMaterial material;
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
        std::vector<MeshInstance> mesh_instances;
    };
    struct DissolutionPerMaterial
    {
        core::math::vec3 albedo_color;
        float metalness;
        float roughness;
        uint32_t flags;
        core::math::vec2 uv_multiplier;
        float time_begin;
        float lifetime;
    };

    auto constexpr dissolution_vs_shader_path = "assets/shaders/dissolution/dissolution-vs.hlsl";
    auto constexpr dissolution_ps_shader_path = "assets/shaders/dissolution/dissolution-ps.hlsl";
    auto constexpr dissolution_vs_depth_only_shader_path = "assets/shaders/dissolution/dissolution-depth-only-vs.hlsl";
    auto constexpr dissolution_gs_depth_only_cubemap_shader_path = "assets/shaders/dissolution/dissolution-depth-only-cubemap-gs.hlsl";
    auto constexpr dissolution_gs_depth_only_texture_shader_path = "assets/shaders/dissolution/dissolution-depth-only-texture-gs.hlsl";
    auto constexpr dissolution_ps_depth_only_shader_path = "assets/shaders/dissolution/dissolution-depth-only-ps.hlsl";

    class DissolutionRenderSystem final : public RenderPass
    {
    public:
        ModelInstance *GetInstancePtr(uint64_t model_id);
        [[nodiscard]] auto &update_time() const { return update_time_; }
        [[nodiscard]] auto &update_time() { return update_time_; }
        DissolutionRenderSystem();
        void OnRender(core::Scene *scene);
        void RenderDepthOnly(std::vector<DissolutionPerDepthCubemap> const &cubemaps, core::Scene *scene);
        void RenderDepthOnly(std::vector<DissolutionPerDepthTexture> const &textures, core::Scene *scene);

        ModelInstance &GetInstance(uint64_t model_id);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, float lifetime, bool appearing = true, bool emissive = true);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, float lifetime, std::vector<DissolutionMaterial> const &materials);

        void Update([[maybe_unused]] core::Scene *scene);
        void ScheduleInstanceUpdate()
        {
            is_instance_update_scheduled_ = true;
        }
    private:
        void UpdateInstances(core::Scene *scene);

        void TransitInstances(core::Scene *scene);

        utils::SteadyTimer update_timer_;
        float update_time_ = 0.1f;

        utils::SteadyTimer particle_spawn_timer_;
        // TODO:
        // This is a hack, we need to make it configurable in the shader
        // This is taken from spawn-particle-vs.hlsl
        static constexpr float kAmountOfCallsPerSecond = 30.0f;

        // TODO:
        void on_attach(entt::registry &, entt::entity) {}
        void on_destroy(entt::registry &, entt::entity) {}
        void on_update(entt::registry &, entt::entity) {}

        bool is_instance_update_scheduled_ = false;

        std::vector<ModelInstance> model_instances_;

        GraphicsShaderProgram dissolution_cubemap_shader_;
        GraphicsShaderProgram dissolution_texture_shader_;
        direct3d::DynamicUniformBuffer<DissolutionPerDepthCubemap> dissolution_per_cubemap_buffer_;
        direct3d::DynamicUniformBuffer<DissolutionPerDepthTexture> dissolution_per_texture_buffer_;

        GraphicsShaderProgram dissolution_shader_;
        direct3d::DynamicUniformBuffer<DissolutionPerMaterial> dissolution_per_material_buffer_;
        direct3d::DynamicUniformBuffer<core::math::mat4> mesh_to_model_buffer_;
        direct3d::DynamicVertexBuffer<DissolutionInstance> instance_buffer_;
        uint32_t instance_buffer_size_;
        ID3D11ShaderResourceView *noise_texture_;
    };
} // namespace engine::render::_dissolution_detail