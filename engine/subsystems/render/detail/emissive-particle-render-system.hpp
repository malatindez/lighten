#pragma once
#include "dissolution-render-system.hpp"
#include "include/library-pch.hpp"
#include "direct3d11/direct3d11.hpp"
#include "misc/sort-lib.hpp"
#include "render/common.hpp"
#include "core/math.hpp"

namespace engine::core
{
    class Scene;
}
// TODO:
// full GPU interpretation
// https://github.com/GPUOpen-LibrariesAndSDKs/GPUParticles11

namespace engine::render
{
    namespace _emissive_particle_detail
    {
        static const uint32_t kMaximumAmountOfParticles = 1000000;

        class EmissiveParticleRenderSystem;

        struct GPUParticle
        {
            core::math::vec3 position;
            float spawn_time;
            core::math::vec3 velocity;
            float lifetime;
            core::math::vec3 color;
            float size;
        };

        struct PerFrame
        {
            uint32_t maximum_amount_of_particles;
            core::math::vec3 padding0;
        };

        struct PerMesh
        {
            core::math::mat4 world_transform;
            float time_begin;
            float object_lifetime;
            core::math::vec2 velocity_range;

            core::math::vec2 size_range;
            float particle_lifetime;
            uint32_t flags;
        };
    } // namespace _emissive_particle_detail

    namespace _emissive_particle_detail
    {
        auto constexpr spawn_particle_vs_shader_path = "assets/shaders/emissive-particle/spawn-particles-vs.hlsl";
        auto constexpr render_particle_vs_shader_path = "assets/shaders/emissive-particle/render-particles-vs.hlsl";
        auto constexpr render_particle_ps_shader_path = "assets/shaders/emissive-particle/render-particles-ps.hlsl";
        auto constexpr update_indirect_args_cs_shader_path = "assets/shaders/emissive-particle/update-indirect-args-cs.hlsl";
        auto constexpr update_particles_cs_shader_path = "assets/shaders/emissive-particle/update-particles-cs.hlsl";
        class EmissiveParticleRenderSystem final : public RenderPass
        {
        public:
            void OnRender(core::Scene *scene, GBuffer const &buffer, ID3D11DepthStencilView *dsv);

            void Update(core::Scene *scene);

            // TODO:
            // fix this
            // System should hold the entity and emit particles until it's not alive, but this is a quick hack
            void EmitParticles(entt::registry &registry, entt::entity entity, std::vector<render::DissolutionMaterial *> const &materials);

            EmissiveParticleRenderSystem();
            ~EmissiveParticleRenderSystem() = default;
            EmissiveParticleRenderSystem(const EmissiveParticleRenderSystem &) = delete;
            EmissiveParticleRenderSystem(EmissiveParticleRenderSystem &&) = delete;
            EmissiveParticleRenderSystem &operator=(const EmissiveParticleRenderSystem &) = delete;
            EmissiveParticleRenderSystem &operator=(EmissiveParticleRenderSystem &&) = delete;

        private:
            void update_particles(direct3d::ShaderResourceView depth_srv, direct3d::ShaderResourceView normals_srv);
            void render_particles(direct3d::ShaderResourceView depth_srv);

            core::math::uivec3 GetParticleRangeData();
            D3D11_DRAW_INSTANCED_INDIRECT_ARGS GetParticleIndirectArgsData();
            std::vector<GPUParticle> GetParticleData();
            GraphicsShaderProgram particle_render_shader_;

            GraphicsShaderProgram particle_emit_shader_;
            ComputeShaderProgram particle_update_shader_;
            ComputeShaderProgram particle_args_update_shader_;

            direct3d::DefaultBuffer<GPUParticle> particle_buffer_;
            direct3d::UnorderedAccessView particle_buffer_uav_;

            direct3d::DefaultBuffer<uint32_t> particle_range_;
            direct3d::UnorderedAccessView particle_range_uav_;

            direct3d::DefaultBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS> particle_indirect_args_;
            direct3d::UnorderedAccessView particle_indirect_args_uav_;

            direct3d::DefaultBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS> particle_indirect_args_copy_;

            direct3d::DynamicUniformBuffer<PerFrame> per_frame_buffer_;
            direct3d::DynamicUniformBuffer<PerMesh> per_mesh_buffer_;
            std::mt19937 random_engine_;
            ID3D11ShaderResourceView *noise_texture_;
        };
    } // namespace _emissive_particle_detail
} // namespace engine::render