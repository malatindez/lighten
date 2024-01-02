#pragma once

#include "../core/reflection-macro.hpp"
#include "core/math.hpp"

// forward declaration
namespace lighten::render::_particle_detail
{
    class ParticleRenderSystem;
}

namespace lighten::components
{
    /**
     * @brief Particle emitter component
     *
     * @warning This component is not thread safe
     * @warning This component is not GPU accelerated
     * @warning This component is not GPU instanced
     *
     * All the angles declared here are relative to the transform.forward of the particle emitter
     * It means that if you want to emit particles in a cone, you need to rotate the transform
     *
     * @todo Add these features to particle-rendering-system:
     * * support for multiple particle materials
     * * PBR
     * * GPU support
     */
    LIGHTEN_COMPONENT(name = "Particle Emitter", category = "Core")
    struct ParticleEmitter final
    {
        /**
         * @brief This is the angle range in which the yaw and pitch will be randomized to define the space where the particles will be emitted
         *
         * The position yaw range is a vector with the following format: (x, y, z, w) = (min_yaw, min_pitch, max_yaw, max_pitch)
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Position Yaw Pitch Range")
        glm::vec4 position_yaw_pitch_range;
        /**
         * @brief This is the radius range in which the particles will be emitted
         *
         * The position radius is a vector with the following format: (x, y) = (min_distance, max_distance)
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Position Radius Range")
        glm::vec2 position_radius;

        /**
         * @brief This is the angle range in which the yaw and pitch will be randomized to rotate the base velocity vector
         *
         * The velocity yaw range is a vector with the following format: (x, y, z, w) = (min_yaw, min_pitch, max_yaw, max_pitch)
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Velocity Yaw Pitch Range")
        glm::vec4 velocity_yaw_pitch_range;
        /**
         * @brief This is the radius range in which the velocity will be randomized
         *
         * The velocity radius is a vector with the following format: (x, y) = (min_velocity, max_velocity)
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Velocity Radius Range")
        glm::vec2 velocity_radius;

        /// @brief Base HDR color of the particle
        LIGHTEN_PROPERTY(serialize, save_game, name = "Base Diffuse Color")
        glm::vec4 base_diffuse_color;
        /**
         * @brief This is the range in which the diffuse color will be randomized
         *
         * The diffuse variation is a vector with the following format: (x, y, z, w) = (r_deviation, g_deviation, b_deviation, a_deviation)
         *
         * It works like this:
         * particle_color = base_diffuse_color + (randomglm::_vec4(-1, 1) * diffuse_variation)
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Diffuse Color Variation")
        glm::vec4 diffuse_variation;

        /**
         * @brief This is the range in which the particle lifespan will be randomized
         *
         * The particle lifespan is a vector with the following format: (x, y) = (min_lifespan, max_lifespan)
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Particle Lifespan Range")
        glm::vec2 particle_lifespan_range;
        /**
         * @brief This is the range in which the particle start size will be randomized
         *
         * The particle start size is a vector with the following format: (x, y) = (min_start_size, max_start_size)
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Begin Size Range")
        glm::vec2 begin_size_range;
        /**
         * @brief This is the range in which the particle end size will be randomized
         *
         * The particle end size is a vector with the following format: (x, y) = (min_end_size, max_end_size)
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "End Size Range")
        glm::vec2 end_size_range;

        /**
         * @brief This is the range in which the particle mass will be randomized
         *
         * The particle mass is a vector with the following format: (x, y) = (min_mass, max_mass)
         *
         * The mass is used to calculate the drag force
         *
         * @warning this parameter is not used!
         * @todo Implement drag force fields
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Mass Range")
        glm::vec2 mass_range;
        /**
         * @brief Thickness of the particle.
         *
         * Thickness is used to calculate the alpha of the particle if there's something behind it
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Thickness Range")
        glm::vec2 thickness_range;

        /**
         * @brief This is the emission rate of the particle emitter per second
         *
         * The emission rate is a float with the following format: (x) = (particles_per_second)
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Emission Rate")
        float emit_rate;

        /**
         * @brief This is the range in which the particle rotation will be randomized
         *
         * @note The rotation is declared in radians
         *
         * The particle rotation is a vector with the following format: (x, y) = (min_rotation, max_rotation)
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Rotation Range")
        glm::vec2 rotation_range;

        /**
         * @brief This is the range in which the particle rotation speed will be randomized
         *
         * @note The rotation speed is declared in radians per second
         *
         * The particle rotation speed is a vector with the following format: (x, y) = (min_rotation_speed, max_rotation_speed)
         *
         * @note There's no reason to set the rotation speed to value outside of the range [\f$-2\pi\f$, \f$2\pi\f$]
         *
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Rotation Speed Range")
        glm::vec2 rotation_speed_range;
        /**
         * @brief This is the acceleration vector of the particles
         *
         * @note This is not the acceleration of the particle emitter, but the acceleration of the particles
         *
         * It is used to apply force to the particles
         *
         * @todo Remove this and apply force fields instead
         *
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Particle Acceleration")
        glm::vec3 particle_acceleration;

        /// @brief Maximum amount of particles that can be alive at the same time for this particle emitter
        LIGHTEN_PROPERTY(serialize, save_game, name = "Maximum Amount of Particles")
        uint32_t maximum_amount_of_particles;

        /// @brief If true, the particle emitter will stop spawning/clearing and updating particles
        LIGHTEN_PROPERTY(serialize, save_game, name = "Freeze")
        bool freeze = false;

    private:
        friend class lighten::render::_particle_detail::ParticleRenderSystem;

        /// @brief CPU particle class
        struct Particle
        {
            /// @brief Position of the particle in the world space
            glm::vec3 position;
            /// @brief Velocity of the particle in the world space
            glm::vec3 velocity;
            /**
             * @brief Acceleration of the particle in the world space
             *
             * @todo Remove this and apply force fields instead
             */
            glm::vec3 acceleration;
            /// @brief Color of the particle
            glm::vec4 color;
            /// @brief Begin size of the particle
            float begin_size;
            /// @brief End size of the particle
            float end_size;
            /// @brief Spawn time of the particle
            float life_begin;
            /// @brief Death time of the particle
            float life_end;
            /// @brief Current rotation angle of the particle
            float rotation;
            /// @brief Rotation speed of the particle
            float rotation_speed;
            /// @brief Thickness of the particle
            float thickness;
        };
        /// @brief List of all the particles that are alive
        std::vector<Particle> particles;
        /**
         * @brief Local counter that is used to properly emit particles with the given emission rate
         *
         * It is used as:
         * last_second_count += dt
         * if last_second_count >= 1.0f: reset last_second_count and particles_last_second_count
         * if particles_last_second_count < last_second_count * emit_rate: emit (last_second_count * emit_rate - particles_last_second_count) particles
         * and add them to particles_last_second_count

         */
        float last_second_count;
        /// @brief Amount of particles that were emitted in the last second
        uint32_t particles_last_second_count;
    };
} // namespace lighten::components
