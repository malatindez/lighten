#pragma once
#include "opaque-render-system.hpp"
namespace engine::core
{
    class Scene;
} // namespace engine::core

namespace engine::render
{
    class ModelSystem;
    namespace _grass_detail
    {
        class GrassRenderSystem;
        struct GrassPerMaterial
        {
            core::math::vec3 albedo_color;
            float ao_value;

            float bump_value;
            float cavity_value;
            float displacement_value;
            float gloss_value;

            float roughness_value;
            core::math::vec3 specular_value;

            float metalness_value;
            core::math::vec3 translucency_value;

            float opacity_value;
            uint32_t plane_count;
            uint32_t section_count;
            uint32_t enabled_texture_flags;

            core::math::vec3 wind_vector;
            float wind_amplitude;
            float wind_wavenumber;
            float wind_frequency;

            core::math::uivec2 atlas_texture_size;
        };
    } // namespace _grass_detail

    struct GrassMaterial
    {
        ID3D11ShaderResourceView *albedo_texture = nullptr;
        ID3D11ShaderResourceView *ao_texture = nullptr;
        ID3D11ShaderResourceView *bump_texture = nullptr;
        ID3D11ShaderResourceView *cavity_texture = nullptr;
        ID3D11ShaderResourceView *displacement_texture = nullptr;
        ID3D11ShaderResourceView *gloss_texture = nullptr;
        ID3D11ShaderResourceView *normal_texture = nullptr;
        ID3D11ShaderResourceView *opacity_texture = nullptr;
        ID3D11ShaderResourceView *roughness_texture = nullptr;
        ID3D11ShaderResourceView *specular_texture = nullptr;
        ID3D11ShaderResourceView *translucency_texture = nullptr;
        ID3D11ShaderResourceView *metalness_texture = nullptr;
        core::math::vec3 albedo_color;
        float ao_value = 0.0f;
        float bump_value = 0.0f;
        float cavity_value = 0.0f;
        float displacement_value = 0.0f;
        float gloss_value = 0.0f;
        float opacity_value = 0.0f;
        float roughness_value = 0.0f;
        core::math::vec3 specular_value = 0.0f;
        core::math::vec3 translucency_value = 0.0f;
        float metalness_value = 0.0f;
        bool reverse_normal_y = false;

        core::math::vec3 wind_vector = core::math::vec3{ 0,0,0 };
        float wind_amplitude = 0.0f;
        float wind_wavenumber = 0.0f;
        float wind_frequency = 0.0f;

        uint32_t texture_flags = 0;
        // from, to texture coordinates
        std::vector<core::math::vec4> atlas_data{};
        core::math::uivec2 atlas_size;

        uint32_t planes_count = 2;
        uint32_t section_count = 3;

        void UpdateTextureFlags();
        GrassMaterial() = default;
        void Bind(direct3d::DynamicUniformBuffer<_grass_detail::GrassPerMaterial> &uniform_buffer) const;
        void BindTextures() const;
        void reset()
        {
            albedo_texture = nullptr;
            ao_texture = nullptr;
            bump_texture = nullptr;
            cavity_texture = nullptr;
            displacement_texture = nullptr;
            gloss_texture = nullptr;
            normal_texture = nullptr;
            opacity_texture = nullptr;
            roughness_texture = nullptr;
            specular_texture = nullptr;
            translucency_texture = nullptr;
            texture_flags = 0;
            planes_count = 1;
            section_count = 1;
            atlas_data.clear();
        }
    };
}

namespace std
{
    template <>
    struct hash<engine::render::GrassMaterial>
    {
        std::size_t operator()(engine::render::GrassMaterial const &material) const
        {
            size_t seed = 0;
            engine::utils::hash_combine(seed, material.albedo_texture);
            engine::utils::hash_combine(seed, material.ao_texture);
            engine::utils::hash_combine(seed, material.bump_texture);
            engine::utils::hash_combine(seed, material.cavity_texture);
            engine::utils::hash_combine(seed, material.displacement_texture);
            engine::utils::hash_combine(seed, material.gloss_texture);
            engine::utils::hash_combine(seed, material.normal_texture);
            engine::utils::hash_combine(seed, material.opacity_texture);
            engine::utils::hash_combine(seed, material.roughness_texture);
            engine::utils::hash_combine(seed, material.specular_texture);
            engine::utils::hash_combine(seed, material.translucency_texture);
            engine::utils::hash_combine(seed, material.metalness_texture);
            engine::utils::hash_combine(seed, material.reverse_normal_y);
            engine::utils::hash_combine(seed, material.texture_flags);
            engine::utils::hash_combine(seed, material.planes_count);
            engine::utils::hash_combine(seed, material.section_count);
            return seed;
        }
    };
} // namespace std

namespace engine::components
{
    struct GrassComponent
    {
        struct GrassInstance
        {
            core::math::vec3 position;
            core::math::vec2 size;
            float rotation;
            uint32_t atlas_id;
        };

        size_t material_id = std::numeric_limits<size_t>::max();
        core::math::vec2 spawn_range = { 2.0f, 2.0f }; // from center
        core::math::vec2 grass_size_range = { 0.1f, 0.5f };
        core::math::vec3 initial_offset = { 0.0f };
        float min_distance;
        uint32_t max_attempts;
        void Initialize(std::vector<core::math::vec4> const &atlas_data);

    private:
        friend class engine::render::_grass_detail::GrassRenderSystem;
        // should be sorted by atlas id
        std::vector<GrassInstance> grass_instances_;
    };
} // namespace engine::components

namespace engine::render::_grass_detail
{
    using GrassPerDepthCubemap = _opaque_detail::OpaquePerDepthCubemap;
    using GrassPerDepthTexture = _opaque_detail::OpaquePerDepthTexture;

    constexpr auto grass_vs_shader_path = "assets/shaders/grass/grass-vs.hlsl";
    constexpr auto grass_ps_shader_path = "assets/shaders/grass/grass-ps.hlsl";
    constexpr auto grass_gs_depth_only_cubemap_shader_path = "assets/shaders/grass/grass-depth-only-cubemap-gs.hlsl";
    constexpr auto grass_gs_depth_only_texture_shader_path = "assets/shaders/grass/grass-depth-only-texture-gs.hlsl";
    constexpr auto grass_ps_depth_only_shader_path = "assets/shaders/grass/grass-depth-only-ps.hlsl";
    struct GPUGrassInstance
    {
        core::math::vec3 position;
        core::math::vec2 size;
        float rotation;
        uint16_t from_uv_x;
        uint16_t from_uv_y;
        uint16_t to_uv_x;
        uint16_t to_uv_y;
    };

    struct MaterialInstance
    {
        std::vector<entt::entity> instances;
        GrassMaterial material;
        size_t material_hash;
    };
    // TODO:
    // make grass dynamically movable by adding transform matrix that contains rotation, scale and position of an object
    // For now, it renders the grass only relatively to the rotation
    class GrassRenderSystem final : public render::RenderPass
    {
    public:
        GrassRenderSystem();
        void OnRender(core::Scene *scene);
        void RenderDepthOnly(std::vector<GrassPerDepthCubemap> const &cubemaps, core::Scene *scene);
        void RenderDepthOnly(std::vector<GrassPerDepthTexture> const &textures, core::Scene *scene);
        void Update([[maybe_unused]] core::Scene *scene) {}
        void ScheduleInstanceUpdate();

        GrassMaterial &GetMaterial(size_t material_id) {
            return materials_[material_id].material;
        }

        /**
         * @brief Adds material to the system.
         * If material already exists does nothing.
         * Otherwise, adds it and returns its hash value.
         * @param material
         * @return grass material id
         */
        size_t AddMaterial(GrassMaterial &&material);
    private:
        struct GPUTransformInfo
        {
            core::math::mat4 rotation_matrix;
            core::math::vec3 position;
            float padding;
        };
        void OnInstancesUpdate(core::Scene *scene);

        bool instances_update_scheduled_ = false;

        GraphicsShaderProgram grass_shader_;
        GraphicsShaderProgram grass_cubemap_shader_;
        GraphicsShaderProgram grass_texture_shader_;

        direct3d::DynamicUniformBuffer<GrassPerDepthCubemap> grass_per_cubemap_buffer_;
        direct3d::DynamicUniformBuffer<GrassPerDepthTexture> grass_per_texture_buffer_;

        direct3d::DynamicUniformBuffer<GPUTransformInfo> grass_transform_buffer_;
        direct3d::DynamicVertexBuffer<GPUGrassInstance> instance_buffer_;

        direct3d::DynamicUniformBuffer<_grass_detail::GrassPerMaterial> grass_per_material_buffer_;

        std::vector<MaterialInstance> materials_;
    };
} // namespace engine::render::_grass_detail