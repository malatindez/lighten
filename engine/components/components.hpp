#pragma once
#include "include/library-pch.hpp"
// forward declaration
namespace engine::render::_particle_detail
{
    class ParticleRenderSystem;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26495) // Variable 'component' is uninitialized. Always initialize a member variable
#endif

/**
 * @brief The components namespace contains all the components that can be used in the engine
 * @todo Every component should inherit from Component class
 * @todo Every component should have static \code{.cpp} constexpr std::string_view name;\endcode member that is used to identify the component by name
 * 
 * If the component doesn't have a name, \code{.cpp}static_assert(false, "Component {class name} must have a name declared");\endcode will be generated automatically
 * in generated file to make sure that the component has a name
 *
 * About forward declaring or including the component headers.
 *
 * Header including can cause issues like this:
 * * increasing the compilation time of the engine
 * * increasing the binary size of the engine
 * * causing problems with the precompiled headers
 * * causing problems with the header include order, because the components-info.hpp
 *          will be included in the engine\components\components.hpp
 *
 * Forward declaration can cause issues like this:
 * * not being able to use the components in the engine\components\components.hpp
 * * not being able to recognize the size of the component before including the header
 * * not being able to use the component in the engine\components\generated\components-info.hpp
 * * still it will be necessary to include the headers in serialization/gui files to use them
 *
 * I think we should include the headers, because it will be easier to work with the components
 *
 * If you don't want to mess up your project you should create separate file for component and/or components
 * Otherwise because they will be included in the engine\components\generated\components-info.hpp file
 * this will lead to problems with header including mentioned above
 *
 @todo Add the python pre-build step that will gather all the components identifying them by Component parent class, and then generate the header files that will contain all the components.
 * This header file will contain `utils::parameter_pack_info<Component, Component, ...>` as the list of types and corresponding `std::array<std::string, N>` as the list of component names
 * It will modify the `engine\components\generated\components-info.hpp` file
 *
 * To iterate through all the component types you can use the following code:
 \code{.cpp}
     utils::for_constexpr<0, components::kAmount, 1>([](auto i) constexpr -> bool {
     using component = components::component_by_id<i>;
     std::cout << component::name << std::endl;
     return true; // if false is returned the loop will break
   });
 \endcode

  * You can use it to substitute the switch statement so you can easily iterate through
  * all the components without worrying about giant and complex switch statement
  * Also, this way, you can easily add new components without modifying the switch statement at all
  * And, most importantly, for_constexpr is equivalent or even faster than the switch statement by 1.5-20% in most cases
  @note But in the most test cases the for_constexpr is better

  @todo Add a python script that will generate the component serialization/deserialization code
  * If you want to generate serialization/deserialization for the component you should add `static constexpr bool kGenerateSerialization = true;` member to the component
  *
  * Private members and members that do not have serialization specification will be ignored.
  * 
  * If you want to create custom serializer you should create a specialization of the `engine::components::Serialize::Serializer`
  * The script will try to find it, but if it won't you can include it in the component header
  * This is the example of a serializer:
  \code{.cpp}
namespace engine::components::Serialize
{
  template<class Ty>
  struct Serializer;
}
  \endcode
  \code{.cpp}
namespace engine::components::Serialize
{
    template<>
    struct Serializer<ComponentName>
    {
        static void Serialize(const ComponentName &component, std::ostream &stream)
        {
            // serialize each of the public members of the component
            // using the engine::components::Serialize::Serializer<T>{} specialization
        }
        static void Deserialize(ComponentName &component, std::istream &stream)
        {
            // deserialize each of the public members of the component
            // using the engine::components::Serialize::Serializer<T>{} specialization
        }
};
  \endcode
  * It will modify the engine\components\generated\components-serialization.hpp file

  @todo Add a python script that will generate the ImGui component editor code
  * This script should create the special configuration file located at `engine\components\generated\components-editor.json`.
  *
  * This file will contain the list of rules how to render each component.
  * 
  * @warning If the member variable cannot be rendered/cast to the `ImGuiEditor<T>{}` specialization it will be written as an "unknown type" in the `.json` file
  * @note The user will be able to edit the `.json` file and add the rules for the `ImGuiEditor<T>{}` specializations
  * @note The user will be able to add the `ImGuiEditor<T>{}` specializations for the types that are not supported by default
  * 
  * Example of `.json` configuration file:
  \code{.json}
{
    "ComponentName": {
        "member1": {
            "name": "member1 name", # name of the member that we should show
            "description": "member1 description",
            "settings": {
               "type": "slider", # type of the editor
               "min": 0.0, # min value for the slider
               "max": 1.0, # max value for the slider
               "step": 0.01, # step value for the slider
               "format": "%.2f", # format string for the slider
               "flags": "ImGuiSliderFlags_Logarithmic" # flags for the slider
            }
        },
        "member2": {	... },
        "member3": { "error": "UNKNOWN TYPE "},
    },
    "ComponentName2": { ... }
}
  \endcode
  * It will work similarly to the serialization/deserialization code
  * If you want to generate editor for the component you should add the
  * `static constexpr bool kGenerateEditor = true;` member to the component
  * This is the example of a component editor:
  \code{.cpp}
namespace engine::components::ImGuiEditor
{
  template<class Ty>
  struct ImGuiEditor;
}
  \endcode
  \code{.cpp}
namespace engine::components::ImGuiEditor
{
    template<>
    struct ImGuiEditor<ComponentName>
    {
        static void Draw(ComponentName &component)
        {
            // draw each of the public members of the component
            // using the engine::components::ImGuiEditor::ImGuiEditor<T>{} specialization
            // It will draw the component in the ImGui window
            // using the ImGui::InputText, ImGui::InputFloat, etc.
            // You can also use the ImGui::DragFloat, ImGui::SliderFloat, etc.
            // to make the component editor more user-friendly
        }
    };
}
  \endcode
  * It will modify the engine\components\generated\components-editor.hpp file
  @todo Add a python script that will generate the `std::hash` extension for each component

  * The generated code will look like this:
  \code{.cpp}
template<>
struct std::hash<engine::components::ComponentName>
{
    std::size_t operator()(const engine::components::ComponentName &component) const noexcept
    {
        std::size_t seed = 0;
        engine::utils::hash_combine (member1);
        engine::utils::hash_combine (member2);
        ...
        engine::utils::hash_combine (memberN);
        // !! each public member should have the std::hash<T>{} specialization
        return seed;
    }
};
  \endcode
  @note Each public member should have the `std::hash<T>{}` specialization to be able to use the hash_combine function
  *
  * If you want to generate hash class for the component you should add the `static constexpr bool kGenerateHash = true;` member to the component.
  *
  * It will modify the engine\components\generated\components-hash.hpp file
*/
namespace engine::components
{
    /// @brief Base class for all components
    struct Component{};
    /**
     * @brief Component that contains the name of the object and its tag
     * @note This component is used to identify the object in the scene and to find it's descendants
     */
    struct GameObject final : Component
    {
        static constexpr bool kGenerateSerialization = false;
        static constexpr bool kGenerateEditor = false;
        static constexpr bool kGenerateHash = false;
        static constexpr std::string_view kName = "Game Object";

        GameObject() = default;
        GameObject(const GameObject &) = default;
        GameObject(GameObject &&) = default;
        GameObject &operator=(const GameObject &) = default;
        GameObject &operator=(GameObject &&) = default;
        ~GameObject() = default;
        /// @brief Object's name
        std::string name = "None";
        /// @brief Object's tag
        std::string tag;
        /// @brief Object's parent
        entt::entity parent = entt::null;
        /// @brief Children of the object
        std::vector<entt::entity> children;
    };
    /// @brief Component that contains the transform of the object
    struct TransformComponent final : Component
    {
        static constexpr bool kGenerateSerialization = true;
        static constexpr bool kGenerateEditor = true;
        static constexpr bool kGenerateHash = false;
        static constexpr std::string_view kName = "Transform";

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
        /// @brief Object position in the local space
        core::math::vec3 position{ 0 };
        /// @brief Object scale in the local space
        core::math::vec3 scale{ 1 };
        /// @brief Object rotation in the local space
        core::math::quat rotation;
        /**
         * @brief Model matrix of the object, basically it's the product of the translation, rotation and scale matrices
         * @note If you want to update member variables you should call registry.patch<TransformComponent>(entity, [](auto &t){}) to notify the systems that use the transform component about the changes in the component
         * @note The systems that want to capture the changes should initialize entt::observer{registry, entt::collector.update<TransformComponent>()} and call observer.each([](auto entity, auto &component){}) to process     the changes
         */
        core::math::mat4 model;
        /// @brief Inverse of the model matrix
        core::math::mat4 inv_model;
        /**
         * @brief World matrix, it is equal to the model matrix of the parent object multiplied by the model matrix of this object
         *
         * @note it is used to calculate the world position of the object
         *
         * @note it is updated in the TransformSystem
         *
         */
        core::math::mat4 world;
        /// @brief Inverse of the world matrix
        core::math::mat4 inv_world;
    };
    /// @brief Component that contains the camera of the object
    struct CameraComponent final : Component
    {
        static constexpr bool kGenerateSerialization = true;
        static constexpr bool kGenerateEditor = false;
        static constexpr bool kGenerateHash = false;
        static constexpr std::string_view kName = "Game Object";

        CameraComponent() = default;
        CameraComponent(const CameraComponent &) = default;
        CameraComponent(CameraComponent &&) = default;
        CameraComponent &operator=(const CameraComponent &) = default;
        CameraComponent &operator=(CameraComponent &&) = default;
        ~CameraComponent() = default;

        /**
         * @brief Returns the right vector of the camera
         *
         * @return right vector of the camera
         */
        [[nodiscard]] constexpr core::math::vec3 right() const noexcept
        {
            return core::math::vec3{ inv_view[0][0], inv_view[0][1], inv_view[0][2] };
        }
        /// @brief Returns the up vector of the camera
        /// @return up vector of the camera
        [[nodiscard]] constexpr core::math::vec3 up() const noexcept
        {
            return core::math::vec3{ inv_view[1][0], inv_view[1][1], inv_view[1][2] };
        }
        /// @brief Returns the forward vector of the camera
        /// @return forward vector of the camera
        [[nodiscard]] constexpr core::math::vec3 forward() const noexcept
        {
            return core::math::vec3{ inv_view[2][0], inv_view[2][1], inv_view[2][2] };
        }
        /// @brief Returns the position of the camera
        /// @return position of the camera
        [[nodiscard]] constexpr core::math::vec3 position() const noexcept
        {
            return core::math::vec3{ inv_view[3][0], inv_view[3][1], inv_view[3][2] };
        }
        /// @brief View matrix
        core::math::mat4 view{ 1 };
        /// @brief Projection matrix
        core::math::mat4 projection{ 1 };
        /// @brief View projection matrix
        core::math::mat4 view_projection{ 1 };
        /// @brief Inverse of the view matrix
        core::math::mat4 inv_view{ 1 };
        /// @brief Inverse of the projection matrix
        core::math::mat4 inv_projection{ 1 };
        /// @brief Inverse of the view projection matrix
        core::math::mat4 inv_view_projection{ 1 };
        /// @brief Field of view in radians
        float fovy_ = core::math::radians(45.0f);
        /// @brief near plane Z coordinate
        float z_near_ = 20000.0f;
        /// @brief far plane Z coordinate
        float z_far_ = 0.002f;
    };

    /// @brief Point light component
    /// TODO: Add these features:
    /// * add casts_shadow boolean so that we can have lights that don't cast shadows
    /// * add shadow_bias
    /// * add shadow_normal_bias
    /// * add shadow_blur
    /// * add shadow_blur_radius
    /// * add shadow_blur_sigma
    struct PointLight final : Component
    {
        static constexpr bool kGenerateSerialization = true;
        static constexpr bool kGenerateEditor = true;
        static constexpr bool kGenerateHash = false;
        static constexpr std::string_view kName = "Game Object";

        PointLight() = default;
        PointLight(const PointLight &) = default;
        PointLight(PointLight &&) = default;
        PointLight &operator=(const PointLight &) = default;
        PointLight &operator=(PointLight &&) = default;
        ~PointLight() = default;

        /// @brief Position of the light in the local space relative to the world matrix in the transform component
        /// @todo
        /// Use position in the LightRenderSystem to calculate the world position of the light
        core::math::vec3 position;
        /// @brief Color of the light
        core::math::vec3 color;
        /// @brief Intensity of light
        float power;

        /// @brief Radius of the light
        /// @todo
        /// Use radius in LightRenderSystem and multiply it by the scale of the transform component
        float radius;
        /// @brief If the light casts shadows
        bool casts_shadows = false;
    };
    /// @brief Directional light component
    struct DirectionalLight final : Component
    {
        DirectionalLight() = default;
        DirectionalLight(const DirectionalLight &) = default;
        DirectionalLight(DirectionalLight &&) = default;
        DirectionalLight &operator=(const DirectionalLight &) = default;
        DirectionalLight &operator=(DirectionalLight &&) = default;
        ~DirectionalLight() = default;
        /// @brief Solid angle of the light, it is used in the shaders to calculate the light intensity
        float solid_angle;
        /// @brief Color of the light
        core::math::vec3 color;
        /// @brief Intensity of light
        float power;
        /// @brief If the light casts shadows
        bool casts_shadows = false;
    };
    /// @brief Spot light component
    struct SpotLight final : Component
    {
        SpotLight() = default;
        SpotLight(const SpotLight &) = default;
        SpotLight(SpotLight &&) = default;
        SpotLight &operator=(const SpotLight &) = default;
        SpotLight &operator=(SpotLight &&) = default;
        ~SpotLight() = default;
        /// @brief Position of the light in the local space relative to the world matrix in the transform component
        /// @todo Use position in the LightRenderSystem to calculate the world position of the light
        core::math::vec3 position;
        /// @brief Radius of the light
        /// @todo Use radius in LightRenderSystem and multiply it by the scale of the transform component
        float radius;
        /// @brief Color of the light
        core::math::vec3 color;
        /// @brief Intensity of light
        float power;
        /// @brief inner cutoff angle in radians
        float inner_cutoff;
        /// @brief outer cutoff angle in radians
        float outer_cutoff;
        /// @brief If the light casts shadows
        bool casts_shadows = false;
    };

    /**
     * @brief Particle emitter component
     *
     * @note This component is not thread safe
     * @note This component is not GPU accelerated
     * @note This component is not GPU instanced
     *
     * All the angles declared here are relative to the transform.forward of the particle emitter
     * It means that if you want to emit particles in a cone, you need to rotate the transform
     *
     * @todo Add these features to particle-rendering-system:
     * * support for multiple particle materials
     * * PBR
     * * GPU support
     */
    struct ParticleEmitter final : Component
    {
        ParticleEmitter() = default;
        ParticleEmitter(const ParticleEmitter &) = default;
        ParticleEmitter(ParticleEmitter &&) = default;
        ParticleEmitter &operator=(const ParticleEmitter &) = default;
        ParticleEmitter &operator=(ParticleEmitter &&) = default;
        ~ParticleEmitter() = default;

        /**
         * @brief This is the angle range in which the yaw and pitch will be randomized to define the space where the particles will be emitted
         *
         * The position yaw range is a vector with the following format: (x, y, z, w) = (min_yaw, min_pitch, max_yaw, max_pitch)
         */
        core::math::vec4 position_yaw_pitch_range;
        /**
         * @brief This is the radius range in which the particles will be emitted
         *
         * The position radius is a vector with the following format: (x, y) = (min_distance, max_distance)
         */
        core::math::vec2 position_radius;

        /**
         * @brief This is the angle range in which the yaw and pitch will be randomized to rotate the base velocity vector
         *
         * The velocity yaw range is a vector with the following format: (x, y, z, w) = (min_yaw, min_pitch, max_yaw, max_pitch)
         */
        core::math::vec4 velocity_yaw_pitch_range;
        /**
         * @brief This is the radius range in which the velocity will be randomized
         *
         * The velocity radius is a vector with the following format: (x, y) = (min_velocity, max_velocity)
         */
        core::math::vec2 velocity_radius;

        /// @brief Base HDR color of the particle
        core::math::vec4 base_diffuse_color;
        /**
         * @brief This is the range in which the diffuse color will be randomized
         *
         * The diffuse variation is a vector with the following format: (x, y, z, w) = (r_deviation, g_deviation, b_deviation, a_deviation)
         *
         * It works like this:
         * particle_color = base_diffuse_color + (random_vec4(-1, 1) * diffuse_variation)
         */
        core::math::vec4 diffuse_variation;

        /**
         * @brief This is the range in which the particle lifespan will be randomized
         *
         * The particle lifespan is a vector with the following format: (x, y) = (min_lifespan, max_lifespan)
         */
        core::math::vec2 particle_lifespan_range;
        /**
         * @brief This is the range in which the particle start size will be randomized
         *
         * The particle start size is a vector with the following format: (x, y) = (min_start_size, max_start_size)
         */
        core::math::vec2 begin_size_range;
        /**
         * @brief This is the range in which the particle end size will be randomized
         *
         * The particle end size is a vector with the following format: (x, y) = (min_end_size, max_end_size)
         */
        core::math::vec2 end_size_range;

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
        core::math::vec2 mass_range;
        /**
         * @brief Thickness of the particle.
         *
         * Thickness is used to calculate the alpha of the particle if there's something behind it
         */
        core::math::vec2 thickness_range;

        /**
         * @brief This is the emission rate of the particle emitter per second
         *
         * The emission rate is a float with the following format: (x) = (particles_per_second)
         */
        float emit_rate;

        /**
         * @brief This is the range in which the particle rotation will be randomized
         *
         * @note The rotation is declared in radians
         *
         * The particle rotation is a vector with the following format: (x, y) = (min_rotation, max_rotation)
         */
        core::math::vec2 rotation_range;

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
        core::math::vec2 rotation_speed_range;
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
        core::math::vec3 particle_acceleration;

        /// @brief Maximum amount of particles that can be alive at the same time for this particle emitter
        uint32_t maximum_amount_of_particles;

        /// @brief If true, the particle emitter will stop spawning/clearing and updating particles
        bool freeze = false;

    private:
        friend class engine::render::_particle_detail::ParticleRenderSystem;

        /// @brief CPU particle class
        struct Particle
        {
            /// @brief Position of the particle in the world space
            core::math::vec3 position;
            /// @brief Velocity of the particle in the world space
            core::math::vec3 velocity;
            /**
             * @brief Acceleration of the particle in the world space
             *
             * @todo Remove this and apply force fields instead
             */
            core::math::vec3 acceleration;
            /// @brief Color of the particle
            core::math::vec4 color;
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
    /// @brief Force field type
    /// @warning This class is unused for now
    /// @todo Implement force fields
    struct ForceField final : Component
    {
        /// @brief Force field type
        enum class Type : uint8_t
        {
            // Point force field is a sphere that applies a force to all the particles inside it
            // Basically works like this: applied_force += force * (1 - (distance / radius) ^ falloff)
            /**
             * @brief Point force field is a sphere that applies a force to all the particles inside of it
             * Works like this: \f$\mathrm{applied_force} \pluseq \mathrm{force} \cdot {{(1 - (\frac{\mathrm{distance}}{\mathrm{radius}})}^\mathrm{falloff}}\f$
             */
            Point = 0,
            // Directional force field is a plane that applies a force to all the particles in front of it
            // Basically works like this: applied_force += direction * force * (1 - (dot(normal, particle_position) / radius) ^ falloff)
            // where direction = transform.forward
            /**
             * @brief Directional force field is a plane that applies a force to all the particles in front of it
             * Works like this: \f$\mathrm{applied_force} \pluseq \mathrm{direction} \cdot \mathrm{force} \cdot {{(1 - (\frac{\mathrm{dot}(\mathrm{normal}, \mathrm{particle_position})}{\mathrm{radius}})}^\mathrm{falloff}}\f$
             */
             Directional = 1,
             // OmniDirectional force field is a sphere field that applies force inwards of this sphere from all directions
             // Can be used to simulate the air/water/whatever resistance
             // You can calculate approximate force for the field like this:
             // 0.5 * fluid_density * average_particle_velocity ^ 2 * average_particle_area * fluid_drag_coefficient
             // It is pretty costy to simulate the whole fluid dynamics, so this is a good approximation
             // Basically works like this: applied_force += -normalized(velocity.direction) * force
             /**
              * @brief OmniDirectional force field is a sphere field that applies force inwards of this sphere from all directions
              * Can be used to simulate the air/water/whatever resistance
              * You can calculate approximate force for the field like this:
              * \f$0.5 \cdot \mathrm{fluid_density} \cdot \mathrm{average_particle_velocity}^2 \cdot \mathrm{average_particle_area} \cdot \mathrm{fluid_drag_coefficient}\f$
              * It is pretty costy to simulate the whole fluid dynamics, so this is a good approximation
              * Works like this: \f$\mathrm{applied_force} \pluseq -\mathrm{normalized}(\mathrm{velocity.direction}) \cdot \mathrm{force}\f$
              */
              OmniDirectional = 2
        };

        ForceField() = default;
        ForceField(const ForceField &) = default;
        ForceField(ForceField &&) = default;
        ForceField &operator=(const ForceField &) = default;
        ForceField &operator=(ForceField &&) = default;
        ~ForceField() = default;
        /// @brief Force field type
        Type type;
        /**
         * @brief Force field strength
         *
         * Negative -> repel, positive -> attract
         */
        float force = 0;
        /// @brief Radius of the force field
        float radius;
        /// @brief Force falloff
        ///
        /// This variable is used to calculate how fast the force decreases with distance
        float falloff;
    };

    /**
     * @brief Collider component
     *
     * @warning This class is unused for now
     * @todo Implement colliders
     *
     */
    struct Collider final : Component
    {
        /// @brief Collider type
        enum class Type : uint8_t
        {
            /// @brief Sphere collider
            Sphere = 0,
            /// @brief Box collider
            Box = 1
        };

        Collider() = default;
        Collider(const Collider &) = default;
        Collider(Collider &&) = default;
        Collider &operator=(const Collider &) = default;
        Collider &operator=(Collider &&) = default;
        ~Collider() = default;
        //// @brief Collider type
        Type type;
        /// half extents of the box collider
        core::math::vec3 half_extents;
        /// radius of the sphere collider
        float radius;
    };
#ifdef _MSC_VER
#pragma warning(pop)
#endif
} // namespace engine::components