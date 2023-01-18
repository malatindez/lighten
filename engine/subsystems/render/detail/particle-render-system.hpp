#pragma once
#include "include/library-pch.hpp"
#include "direct3d11/direct3d11.hpp"
#include "misc/sort-lib.hpp"
#include "render/common.hpp"
#include "core/math.hpp"

#ifndef PARTICLE_RENDER_SYSTEM_AMOUNT_OF_EMITTER_PER_CALL
#define PARTICLE_RENDER_SYSTEM_AMOUNT_OF_EMITTER_PER_CALL 16
#endif

namespace engine::core
{
    class Scene;
}
// TODO:
// full GPU interpretation
// https://github.com/GPUOpen-LibrariesAndSDKs/GPUParticles11

namespace engine::render
{
    namespace _particle_detail
    {
        static constexpr uint32_t kMaximumEmitterAmountPerFrame = PARTICLE_RENDER_SYSTEM_AMOUNT_OF_EMITTER_PER_CALL;

        class ParticleRenderSystem;
 
        constexpr uint32_t kParticleShaderMaxPointLights = 32;
        constexpr uint32_t kParticleShaderMaxSpotLights = 32;
        constexpr uint32_t kParticleShaderMaxDirectionalLights = 4;
        
        struct GPUParticle
        {
            core::math::vec3 position;
            core::math::vec3 velocity;
            core::math::vec3 acceleration; // e.g. gravity
            core::math::vec4 color;
            float begin_size;
            float end_size;
            float rotation;
            float rotation_speed;
            float lifespan;
            float thickness;
        };

        struct ParticlePerFrame
        {
            std::array<GPUPointLight, kParticleShaderMaxPointLights> point_lights;
            std::array<GPUSpotLight, kParticleShaderMaxSpotLights> spot_lights;
            std::array<GPUDirectionalLight, kParticleShaderMaxDirectionalLights> directional_lights;
            uint32_t num_point_lights;
            uint32_t num_spot_lights;
            uint32_t num_directional_lights;
            // Used to interpolate particles between frames
            float time_since_last_tick;
            uint32_t atlas_size_x;
            uint32_t atlas_size_y;
            uint32_t use_dms_depth_texture;
            float padding;
        };
    }
    // TODO:
    // extend to support multiple textures
    // move to GPU

    namespace _particle_detail
    {
        auto constexpr particle_vs_shader_path = "assets/shaders/particle/particle-vs.hlsl";
        auto constexpr particle_ps_shader_path = "assets/shaders/particle/particle-ps.hlsl";
        class ParticleRenderSystem : public RenderPass
        {
        public:
            void OnRender(core::Scene *scene) override;
            void Tick(core::Scene *scene, float delta_time);
            ParticleRenderSystem();
            ~ParticleRenderSystem() = default;
            ParticleRenderSystem(const ParticleRenderSystem &) = delete;
            ParticleRenderSystem(ParticleRenderSystem &&) = delete;
            ParticleRenderSystem &operator=(const ParticleRenderSystem &) = delete;
            ParticleRenderSystem &operator=(ParticleRenderSystem &&) = delete;

            ID3D11ShaderResourceView *botbf = nullptr;
            ID3D11ShaderResourceView *scatter = nullptr;
            ID3D11ShaderResourceView *emva1 = nullptr;
            ID3D11ShaderResourceView *emva2 = nullptr;
            ID3D11ShaderResourceView *rlt = nullptr;

            core::math::uivec2 atlas_size = { 1, 1 };
        private:
            GraphicsShaderProgram particle_shader_;
            direct3d::DynamicUniformBuffer<ParticlePerFrame> particle_per_frame_buffer_;
            direct3d::DynamicVertexBuffer<GPUParticle> particle_buffer_;
            float last_tick_time_ = 0.0f;
            std::mt19937 random_engine_;
        };
    } // namespace _particle_detail
} // namespace engine::render