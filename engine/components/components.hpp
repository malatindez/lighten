#pragma once
#include "include/library-pch.hpp"
namespace engine::components
{
#pragma warning(push)
#pragma warning(disable : 26495)
    struct GameObject final
    {
        GameObject() = default;
        GameObject(const GameObject &) = default;
        GameObject(GameObject &&) = default;
        GameObject &operator=(const GameObject &) = default;
        GameObject &operator=(GameObject &&) = default;
        ~GameObject() = default;
        std::string name;
        std::string tag;
        // TODO
        // class Entity that will have pointer to entt::registry and will take care of
        // updating GameObject and handling components
        //        entt::entity parent;
        //        std::vector<entt::entity> children;
        //        std::vector<std::string> components;
    };
    struct TransformComponent final
    {
        TransformComponent() = default;
        TransformComponent(const TransformComponent &) = default;
        TransformComponent(TransformComponent &&) = default;
        TransformComponent &operator=(const TransformComponent &) = default;
        TransformComponent &operator=(TransformComponent &&) = default;
        ~TransformComponent() = default;
        void reset()
        {
            position = core::math::vec3{ 0 };
            scale = core::math::vec3{ 1 };
            rotation = core::math::quat{ 1, 0, 0, 0 };
            UpdateMatrices();
        }
        void UpdateMatrices() noexcept
        {
            model = core::math::mat4{ 1 };
            model = core::math::translate(model, position);
            model = model * rotation.as_mat4();
            model = core::math::scale(model, scale);
            inv_model = core::math::inverse(model);
        }
        core::math::vec3 position{ 0 };
        float padding0 = 0;
        core::math::vec3 scale{ 1 };
        float padding1 = 0;
        core::math::quat rotation;
        core::math::mat4 model;
        core::math::mat4 inv_model;
    };
#pragma warning(pop)

    struct CameraComponent final
    {
        CameraComponent() = default;
        CameraComponent(const CameraComponent &) = default;
        CameraComponent(CameraComponent &&) = default;
        CameraComponent &operator=(const CameraComponent &) = default;
        CameraComponent &operator=(CameraComponent &&) = default;
        ~CameraComponent() = default;

        constexpr core::math::vec3 right() const noexcept
        {
            return core::math::vec3{ inv_view[0][0], inv_view[0][1], inv_view[0][2] };
        }
        constexpr core::math::vec3 up() const noexcept
        {
            return core::math::vec3{ inv_view[1][0], inv_view[1][1], inv_view[1][2] };
        }
        constexpr core::math::vec3 forward() const noexcept
        {
            return core::math::vec3{ inv_view[2][0], inv_view[2][1], inv_view[2][2] };
        }
        constexpr core::math::vec3 position() const noexcept
        {
            return core::math::vec3{ inv_view[3][0], inv_view[3][1], inv_view[3][2] };
        }

        core::math::mat4 view{ 1 };
        core::math::mat4 projection{ 1 };
        core::math::mat4 view_projection{ 1 };
        core::math::mat4 inv_view{ 1 };
        core::math::mat4 inv_projection{ 1 };
        core::math::mat4 inv_view_projection{ 1 };
        float fovy_ = core::math::radians(45.0f);
        float z_near_ = 20000.0f;
        float z_far_ = 0.002f;
    };
    struct PointLight final
    {
        PointLight() = default;
        PointLight(const PointLight &) = default;
        PointLight(PointLight &&) = default;
        PointLight &operator=(const PointLight &) = default;
        PointLight &operator=(PointLight &&) = default;
        ~PointLight() = default;

        core::math::vec3 color;
        float power;
    };
    struct DirectionalLight final
    {
        DirectionalLight() = default;
        DirectionalLight(const DirectionalLight &) = default;
        DirectionalLight(DirectionalLight &&) = default;
        DirectionalLight &operator=(const DirectionalLight &) = default;
        DirectionalLight &operator=(DirectionalLight &&) = default;
        ~DirectionalLight() = default;
        // direction is determined by transform
        float solid_angle;
        core::math::vec3 color;
        float power;
    };
    struct SpotLight final
    {
        SpotLight() = default;
        SpotLight(const SpotLight &) = default;
        SpotLight(SpotLight &&) = default;
        SpotLight &operator=(const SpotLight &) = default;
        SpotLight &operator=(SpotLight &&) = default;
        ~SpotLight() = default;

        core::math::vec3 color;
        float power;
        // direction is determined by transform
        float inner_cutoff;
        float outer_cutoff;
    };

    struct ParticleEmitter final
    {
        ParticleEmitter() = default;
        ParticleEmitter(const ParticleEmitter &) = default;
        ParticleEmitter(ParticleEmitter &&) = default;
        ParticleEmitter &operator=(const ParticleEmitter &) = default;
        ParticleEmitter &operator=(ParticleEmitter &&) = default;
        ~ParticleEmitter() = default;
        
        // All the angles are relative to the transform.forward of the particle emitter
        // It means that if you want to emit particles in a cone, you need to rotate the transform

        // The position yaw range is a vector with the following format: (x, y, z, w) = (min_yaw, min_pitch, max_yaw, max_pitch)
        // range in which the yaw and pitch will be randomized to rotate the base position vector
        core::math::vec4 position_yaw_pitch_range;
        // range in which the position will be randomized
        core::math::vec2 position_range;
        
        // The velocity yaw range is a vector with the following format: (x, y, z, w) = (min_yaw, min_pitch, max_yaw, max_pitch)
        // range in which the yaw and pitch will be randomized to rotate the base velocity vector
        core::math::vec4 velocity_yaw_pitch_range; 
        // range in which the velocity will be randomized
        core::math::vec2 velocity_range;
        
        // Base HDR color of the generated particle
        core::math::vec4 base_diffuse_value;
        // Basically works like this: base_diffuse_value + (random_color_v4 * diffuse_variation)
        core::math::vec4 diffuse_variation;

        // range in which the particle life span will be randomized
        core::math::vec2 particle_lifespan_range;
        
        // range in which the start size will be randomized
        core::math::vec2 start_size_range;
        // range in which the end size will be randomized
        core::math::vec2 end_size_range; 

        // range in which the mass will be randomized
        // mass is used to calculate the drag force
        core::math::vec2 mass_range; 
        
        // range in which the emit rate will be randomized
        core::math::vec2 emit_rate_range; 

        // Maximum amount of particles that can be alive at the same time for this particle emitter
        int maximum_amount_of_particles = 1000;
        core::math::vec2 spawn_rate_range;
    };

    struct ForceField final
    {
        enum class Type : uint8_t
        {
            // Point force field is a sphere that applies a force to all the particles inside it
            // Basically works like this: force * (1 - (distance / radius) ^ falloff)
            Point = 0,
            // Directional force field is a plane that applies a force to all the particles in front of it
            // Basically works like this: direction * force * (1 - (dot(normal, particle_position) / radius) ^ falloff)
            // where direction = transform.forward
            Directional = 1
        };
        
        ForceField() = default;
        ForceField(const ForceField &) = default;
        ForceField(ForceField &&) = default;
        ForceField &operator=(const ForceField &) = default;
        ForceField &operator=(ForceField &&) = default;
        ~ForceField() = default;

        Type type;
        // negative -> repel, positive -> attract
        float force = 0;
        // radius of the force field
        float radius;
        // how fast the force decreases with distance
        float falloff;
    };
    
    struct Collider final
    {
        enum class Type : uint8_t
        {
            Sphere = 0,
            Box = 1
        };

        Collider() = default;
        Collider(const Collider &) = default;
        Collider(Collider &&) = default;
        Collider &operator=(const Collider &) = default;
        Collider &operator=(Collider &&) = default;
        ~Collider() = default;

        Type type;
        // half extents of the box collider
        core::math::vec3 half_extents;
        // radius of the sphere collider
        float radius;
    };
} // namespace engine::components