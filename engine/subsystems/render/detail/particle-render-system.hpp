#pragma once
#include "include/library-pch.hpp"
#include "direct3d11/direct3d11.hpp"
#include "misc/sort-lib.hpp"

#ifndef PARTICLE_RENDER_SYSTEM_AMOUNT_OF_EMITTER_PER_CALL
#define PARTICLE_RENDER_SYSTEM_AMOUNT_OF_EMITTER_PER_CALL 16
#endif

namespace engine::core
{
    class Scene;
}

namespace engine::render
{
    namespace _particle_detail
    {
        static constexpr uint32_t kMaximumEmitterAmountPerFrame = PARTICLE_RENDER_SYSTEM_AMOUNT_OF_EMITTER_PER_CALL;
            
        class ParticleRenderSystem;
        struct GPUParticleEmitter
        {
            // The position is a vector with the following format: (x, y, z, w) = (min_yaw, min_pitch, max_yaw, max_pitch)
            core::math::vec4 position_yaw_pitch_range;
            // The velocity is a vector with the following format: (x, y, z, w) = (min_yaw, min_pitch, max_yaw, max_pitch)
            core::math::vec4 velocity_yaw_pitch_range;

            // The base color of the particle, the color will be randomized using the diffuse_variation
            core::math::vec4 base_diffuse_value;
            // Basically works like this: base_diffuse_value + (random_color_v4 * diffuse_variation)
            core::math::vec4 diffuse_variation;

            // range in which the position vector will be randomized
            core::math::vec2 position_range;
            // range in which the velocity vector will be randomized
            core::math::vec2 velocity_range;

            // range in which the particle life span will be randomized
            core::math::vec2 lifespan_range;
            
            // Size at the beginning of the particle's life
            core::math::vec2 start_size_range; 
            // Size of the particle at the end of it's life
            core::math::vec2 end_size_range;

            // Mass of the particle (used to simulate collisions if enabled)
            core::math::vec2 mass_range;

            // Maximum amount of particles that can be alive at the same time
            int maximum_amount_of_particles;

            // Amount of particles that will be spawned per second
            core::math::vec2 spawn_rate_range;

            float padding;

            core::math::mat4 rotation_matrix;
            core::math::mat4 world_transform;
        };
        // Force field used to simulate gravity, wind, etc.
        // The force field is a sphere with a direction and a strength
        struct GPUPointForceField
        {
            // The position is the center of the sphere
            core::math::vec3 position;
            // The force is the amount of force applied to the particle
            float force;
            // The radius is the distance from the center of the sphere where the force is applied
            float radius;
            // how fast the force decreases with distance
            float falloff;
            core::math::vec2 padding;
        };
        // Directional force field is a plane that applies a force to all the particles in front of it
        // Basically works like this: if (dot(direction, particle_pos - position) > 0) then apply force
        // force = direction * force * (1 - dot(direction, particle_pos - position) ** falloff)
        
        struct GPUDirectionalForceField
        {
            // The force is the amount of force applied to the particle
            float force;
            // The direction is the direction of the plane, basically transform.forward
            core::math::vec3 direction;    
            // radius of the force field
            float radius;
            // how fast the force decreases with distance
            float falloff;
            core::math::vec2 padding;
        };

        struct GPUParticleRenderSystem
        {
            std::array<GPUParticleEmitter, kMaximumEmitterAmountPerFrame> emitters;
            uint32_t emitter_count = 0;
            float delta_time;
            float time_from_start;
            uint32_t max_particles_this_frame;
        };
        // Part used for rendering
        struct GPUParticlePartA
        {
            core::math::vec4 color;
            core::math::vec2 size;
            float thickness;
            float rotation;
        };
        // Part used for compute shader
        struct GPUParticlePartB
        {
            core::math::vec3 posWS; // world-space position
            float mass;
            core::math::vec3 velocity;
            float lifespan;
            core::math::vec3 acceleration;
            float age; // The currenge age of the particle counting down from lifespan to 0 in seconds

            // 0-15 bits: index of the emitter
            // 16-23 bits: atlas index
            uint32_t properties;
            float distance_to_eye;
            float start_size;
            float end_size;

            float rotation_speed;
            core::math::vec3 padding0;
        };
    }

    namespace _particle_detail
    {
        // based on AMD GPU Particles Sample
        // https://github.com/GPUOpen-LibrariesAndSDKs/GPUParticles11
        class ParticleRenderSystem
        {
        public:
            void Render(core::Scene *scene);
            void OnInstancesUpdated(core::Scene *scene);
            ParticleRenderSystem() = default;
            ~ParticleRenderSystem() = default;
            ParticleRenderSystem(const ParticleRenderSystem &) = delete;
            ParticleRenderSystem(ParticleRenderSystem &&) = delete;
            ParticleRenderSystem &operator=(const ParticleRenderSystem &) = delete;
            ParticleRenderSystem &operator=(ParticleRenderSystem &&) = delete;

            void Init(uint32_t max_particle_count, uint32_t random_texture_size);


            static constexpr size_t CalculateGPUMemoryNeeds(size_t size)
            {
                return size * (sizeof(GPUParticlePartA) + sizeof(GPUParticlePartB) + sizeof(core::math::vec4) + sizeof(uint32_t));
            }

            inline size_t AllocatedGPUMemory()
            {
                return CalculateGPUMemoryNeeds(max_particle_count_);
            }

        private:

            void FillRandomTexture();
            void Sort()
            {
                // Sort the particles by distance to the camera
                sort_lib_.Run(max_particle_count_, alive_list_uav_.ptr(), alive_list_constant_buffer_.buffer());
            }

            uint32_t random_texture_size_ = 1024;
            // Maximum amount of particles that can be alive at the same time
            // 512000
            uint32_t max_particle_count_ = 1024 * 500;
            
            misc::SortLib sort_lib_;

            direct3d::Texture2D random_texture_;
            direct3d::ShaderResourceView random_texture_srv_;

            direct3d::DefaultBuffer<GPUParticlePartA> particle_a_buffer_;
            direct3d::ShaderResourceView particle_a_srv_;
            direct3d::UnorderedAccessView particle_a_uav_;

            direct3d::DefaultBuffer<GPUParticlePartB> particle_b_buffer_;
            direct3d::ShaderResourceView particle_b_srv_;
            direct3d::UnorderedAccessView particle_b_uav_;

            direct3d::DefaultBuffer<core::math::vec4> viewspace_particle_positions_buffer_;
            direct3d::ShaderResourceView viewspace_particle_positions_srv_;
            direct3d::UnorderedAccessView viewspace_particle_positions_uav_;

            direct3d::DefaultBuffer<uint32_t> dead_list_buffer_;
            direct3d::ShaderResourceView dead_list_srv_;
            direct3d::UnorderedAccessView dead_list_uav_;
#if _DEBUG
            direct3d::StagingBuffer<uint32_t> debug_counter_buffer_;
#endif

            direct3d::DefaultBuffer<core::math::uivec4> dead_list_constant_buffer_;
            direct3d::DefaultBuffer<core::math::uivec4> alive_list_constant_buffer_;

            struct AliveParticle
            {
                uint32_t index;
                float distance_to_eye;
            };

            direct3d::DefaultBuffer<AliveParticle> alive_list_buffer_;
            direct3d::ShaderResourceView alive_list_srv_;
            direct3d::UnorderedAccessView alive_list_uav_;

            direct3d::BlendState blend_state_;

            direct3d::DynamicUniformBuffer<GPUParticleRenderSystem> gpu_particle_render_system_buffer_;

            direct3d::ComputeShader init_dead_list_shader_;
            
            std::vector<entt::entity, GPUParticleEmitter> emitters_;
            
            utils::HighResolutionTimer delta_time_timer_;
        };
    }
}
