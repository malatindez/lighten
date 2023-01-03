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
        core::math::vec3 albedo_color;
        float metalness_value;
        float roughness_value;
        uint32_t texture_flags;
        bool reverse_normal_y = false;
        core::math::vec2 uv_multiplier{ 1 };

        void UpdateTextureFlags();
        DissolutionMaterial() = default;
        void Bind(direct3d::DynamicUniformBuffer<_dissolution_detail::DissolutionPerMaterial> &uniform_buffer) const;
        explicit DissolutionMaterial(Material const &material);
        void reset()
        {
            albedo_map = nullptr;
            normal_map = nullptr;
            metalness_map = nullptr;
            roughness_map = nullptr;
            albedo_color = core::math::vec3{ 0.0f };
            metalness_value = 0.01f;
            roughness_value = 0.01f;
            texture_flags = 0;
            reverse_normal_y = false;
            uv_multiplier = core::math::vec2{ 1 };
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
            engine::utils::hash_combine(seed, material.texture_flags);
            engine::utils::hash_combine(seed, material.reverse_normal_y);
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
    using DissolutionPerFrame = _opaque_detail::OpaquePerFrame;
    using DissolutionPerDepthCubemap = _opaque_detail::OpaquePerDepthCubemap;
    using DissolutionPerDepthTexture = _opaque_detail::OpaquePerDepthTexture;
    using _opaque_detail::kOpaqueShaderMaxPointLights;
    using _opaque_detail::kOpaqueShaderMaxSpotLights;
    using _opaque_detail::kOpaqueShaderMaxDirectionalLights;

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
        uint32_t enabled_texture_flags;
        core::math::vec2 uv_multiplier;
        float time_begin;
        float lifetime;
    };

    auto constexpr dissolution_vs_shader_path = "assets/shaders/dissolution/dissolution-vs.hlsl";
    auto constexpr dissolution_ps_shader_path = "assets/shaders/dissolution/dissolution-ps.hlsl";
    auto constexpr dissolution_vs_depth_only_shader_path = "assets/shaders/dissolution/dissolution-depth-only-vs.hlsl";
    auto constexpr dissolution_gs_depth_only_cubemap_shader_path = "assets/shaders/dissolution/dissolution-depth-only-cubemap-gs.hlsl";
    auto constexpr dissolution_gs_depth_only_texture_shader_path = "assets/shaders/dissolution/dissolution-depth-only-texture-gs.hlsl";

    class DissolutionRenderSystem : public RenderPass
    {
    public:
        ModelInstance *GetInstancePtr(uint64_t model_id);
        void SetIrradianceTexture(ID3D11ShaderResourceView *texture) { irradiance_texture_ = texture; }
        void SetPrefilteredTexture(ID3D11ShaderResourceView *texture) { prefiltered_texture_ = texture; }
        void SetBrdfTexture(ID3D11ShaderResourceView *texture) { brdf_texture_ = texture; }
        void SetAmbientOcclusionValue(float value) { ambient_occlusion_value_ = value; }
        [[nodiscard]] ID3D11ShaderResourceView *GetIrradianceTexture() const { return irradiance_texture_; }
        [[nodiscard]] ID3D11ShaderResourceView *GetPrefilteredTexture() const { return prefiltered_texture_; }
        [[nodiscard]] ID3D11ShaderResourceView *GetBrdfTexture() const { return brdf_texture_; }
        [[nodiscard]] float const &ambient_occlusion() const { return ambient_occlusion_value_; }
        [[nodiscard]] float &ambient_occlusion() { return ambient_occlusion_value_; }
        [[nodiscard]] auto &update_time() const { return update_time_; }
        [[nodiscard]] auto &update_time() { return update_time_; }
        DissolutionRenderSystem();
        void OnRender(core::Scene *scene) override;
        void RenderDepthOnly(std::vector<DissolutionPerDepthCubemap> const &cubemaps, core::Scene *scene);
        void RenderDepthOnly(std::vector<DissolutionPerDepthTexture> const &textures, core::Scene *scene);

        ModelInstance &GetInstance(uint64_t model_id);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, float lifetime);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<DissolutionMaterial> const &materials, float lifetime);

        void Update([[maybe_unused]] core::Scene *scene);
        void ScheduleOnInstancesUpdate()
        {
            should_update_instances_ = true;
        }
    private:
        void OnInstancesUpdated(core::Scene *scene);

        void TransitInstances(core::Scene *scene);

        utils::SteadyTimer update_timer_;
        float update_time_ = 0.1f;

        // TODO:
        void on_attach(entt::registry &, entt::entity) {}
        void on_destroy(entt::registry &, entt::entity) {}
        void on_update(entt::registry &, entt::entity) {}

        bool should_update_instances_ = false;

        std::vector<ModelInstance> model_instances_;

        GraphicsShaderProgram dissolution_cubemap_shader_;
        GraphicsShaderProgram dissolution_texture_shader_;
        direct3d::DynamicUniformBuffer<DissolutionPerDepthCubemap> dissolution_per_cubemap_buffer_;
        direct3d::DynamicUniformBuffer<DissolutionPerDepthTexture> dissolution_per_texture_buffer_;

        GraphicsShaderProgram dissolution_shader_;
        direct3d::DynamicUniformBuffer<DissolutionPerFrame> dissolution_per_frame_buffer_;
        direct3d::DynamicUniformBuffer<DissolutionPerMaterial> dissolution_per_material_buffer_;
        direct3d::DynamicUniformBuffer<core::math::mat4> mesh_to_model_buffer_;
        direct3d::DynamicVertexBuffer<DissolutionInstance> instance_buffer_;
        ID3D11ShaderResourceView *irradiance_texture_;
        ID3D11ShaderResourceView *prefiltered_texture_;
        ID3D11ShaderResourceView *brdf_texture_;
        ID3D11ShaderResourceView *noise_texture_;
        float ambient_occlusion_value_ = 1.0f;
    };
} // namespace engine::render::_dissolution_detail