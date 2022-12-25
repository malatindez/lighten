#pragma once
#include "include/library-pch.hpp"
// forward declaration
namespace engine::render::_particle_detail
{
    class ParticleRenderSystem;
}

// TODO:
// make general empty class Component
// Every component should inherit from it and have a
// static constexpr std::string_view `name` member that is used to identify the component by name
// If it doesn't have a name, static_assert(false, "Component must have a name"); will be used
// to make sure that the component has a name

// Should we just forward declare components or include the headers?

// Header including can cause issues like this:
// * increasing the compilation time of the engine
// * increasing the binary size of the engine
// * causing problems with the precompiled headers
// * causing problems with the header include order, because the components-info.hpp
//          will be included in the engine\components\components.hpp

// forward declaration can cause issues like this:
// * not being able to use the components in the engine\components\components.hpp
// * not being able to recognize the size of the component before including the header
// * not being able to use the component in the engine\components\generated\components-info.hpp
// * still it will be necessary to include the headers in serialization/gui files to use them

// I think we should include the headers, because it will be easier to work with the components

// If you don't want to mess up your project you should create separate file for component and/or components
// Because they will be included in the engine\components\generated\components-info.hpp file
// and will lead to problems with header including mentioned above

// TODO:
// Add the python pre-build step that will gather all the components identifying them
// by Component parent class, and then generate the header files that will contain all the components.
// It will contain
// utils::parameter_pack_info<Component, Component, ...> as the list of types and
// corresponding std::array<std::string, N> as the list of component names
// It will modify the engine\components\generated\components-info.hpp file

// To iterate through all the component types you can use the following code:
// utils::for_constexpr<0, components::kAmount, 1>([](auto i) constexpr -> bool { 
//   using component = components::component_by_id<i>;
//   std::cout << component::name << std::endl;
//   return true; // if false is returned the loop will break
// });

// You can use it to substitute the switch statement so you can easily iterate through 
// all the components without worrying about giant and complex switch statement
// Also, this way, you can easily add new components without modifying the switch statement at all
// And, most importantly, for_constexpr is equivalent or even faster than the
// switch statement by 1.5-20% (according to my tests using google benchmark):
// In one test using the intel C++ compiler the switch statement was 6.02% faster
// than the for_constexpr loop
// but in the most tests the for_constexpr is faster


// Faster by ~8.5% on quick-bench.com:
// https://quick-bench.com/q/kECfb_lV71FRuVmVp8t3JAE7IYg
// MSVC i5-12600kf:                    constexpr better by 13.08%
// Benchmark                    Time             CPU   Iterations
// --------------------------------------------------------------
// ConstexprFor              93.0 ns         94.2 ns      7466667
// LongSwitchStatement        107 ns          109 ns      7466667
// --------------------------------------------------------------
// ICC i5-12600kf:                      constexpr better by 3.73%
// --------------------------------------------------------------
// Benchmark                    Time             CPU   Iterations
// --------------------------------------------------------------
// ConstexprFor              20.6 ns         20.9 ns     34461538
// LongSwitchStatement       21.4 ns         21.5 ns     32000000

// quick-bench:                         constexpr better by 0.64%
// https://quick-bench.com/q/63ZalWJ_xTZZ-fCTzPn3fumE6ns
// MSVC i5-12600kf:                     cosntexpr better by 1.33%
// --------------------------------------------------------------
// ConstexprFor              29.7 ns         29.3 ns     22400000
// LongSwitchStatement       30.1 ns         29.8 ns     23578947
// --------------------------------------------------------------
// ICC i5-12600kf:                      cosntexpr better by 0.72%
// --------------------------------------------------------------
// Benchmark                    Time             CPU   Iterations
// --------------------------------------------------------------
// ConstexprFor              13.8 ns         13.8 ns     49777778
// LongSwitchStatement       13.9 ns         14.0 ns     56000000


// quick-bench:                        constexpr better by 14.07%
// https://quick-bench.com/q/sviPz85GilYvdmZiIbMP96ybXzU
// MSVC i5-12600kf:                     constexpr better by 8.32%
// --------------------------------------------------------------
// Benchmark                    Time             CPU   Iterations
// --------------------------------------------------------------
// ConstexprFor              86.0 ns         85.8 ns      7466667
// LongSwitchStatement       93.8 ns         94.2 ns      7466667
// ICC i5-12600kf:                         switch better by 6.02% !!!!
// --------------------------------------------------------------
// Benchmark                    Time             CPU   Iterations
// --------------------------------------------------------------
// ConstexprFor              26.6 ns         26.7 ns     26352941
// LongSwitchStatement       25.0 ns         24.6 ns     29866667


// TODO:
// Add a python script that will generate the component serialization/deserialization code
// If you want to generate serialization/deserialization for the component you should add the
// static constexpr bool kGenerateSerialization = true; member to the component
// This is the example of a serializer:
// namespace engine::components::Serialize
// {
//   template<class Ty>
//   struct Serializer;
// }
// namespace engine::components::Serialize
// {
//     template<>
//     struct Serializer<ComponentName>
//     {
//         static void Serialize(const ComponentName &component, std::ostream &stream)
//         {
//             // serialize each of the public members of the component
//             // using the engine::components::Serialize::Serializer<T>{} specialization
//         }
//         static void Deserialize(ComponentName &component, std::istream &stream)
//         {
//             // deserialize each of the public members of the component
//             // using the engine::components::Serialize::Serializer<T>{} specialization
//         }
// };
// It will modify the engine\components\generated\components-serialization.hpp file


// TODO:
// Add a python script that will generate the ImGui component editor code

// It will create the special configuration file engine\components\generated\components-editor.json
// that will contain the list of rules how to render each component
// If the member variable cannot be rendered/cast to the ImGuiEditor<T>{} specialization it will be written as an "unknown type" in the json file
// The user will be able to edit the json file and add the rules for the ImGuiEditor<T>{} specializations
// The user will be able to add the ImGuiEditor<T>{} specializations for the types that are not supported by default
// Example of json file
// {
//     "ComponentName": {
//         "member1": {
//             "name": "member1 name", # name of the member that we should show
//             "description": "member1 description",
//             "settings": {
//                "type": "slider", # type of the editor
//                "min": 0.0, # min value for the slider
//                "max": 1.0, # max value for the slider
//                "step": 0.01, # step value for the slider
//                "format": "%.2f", # format string for the slider
//                "flags": "ImGuiSliderFlags_Logarithmic" # flags for the slider
//             }
//         },
//         "member2": {	... },
//         "member3": { "error": "UNKNOWN TYPE "},
//     },
//     "ComponentName2": { ... }
// }

// It will work similarly to the serialization/deserialization code
// If you want to generate editor for the component you should add the
// static constexpr bool kGenerateEditor = true; member to the component
// This is the example of a component editor:
// namespace engine::components::ImGuiEditor
// {
//   template<class Ty>
//   struct ImGuiEditor;
// }
// namespace engine::components::ImGuiEditor
// {
//     template<>
//     struct ImGuiEditor<ComponentName>
//     {
//         static void Draw(ComponentName &component)
//         {
//             // draw each of the public members of the component
//             // using the engine::components::ImGuiEditor::ImGuiEditor<T>{} specialization
//             // It will draw the component in the ImGui window 
//             // using the ImGui::InputText, ImGui::InputFloat, etc.
//             // You can also use the ImGui::DragFloat, ImGui::SliderFloat, etc.
//             // to make the component editor more user-friendly
//         }
//     };
// }
// It will modify the engine\components\generated\components-editor.hpp file

// TODO:
// Add a python script that will generate the std::hash extension for each component
// It will work mostly like this:
// template<>
// struct std::hash<engine::components::ComponentName>
// {
//     std::size_t operator()(const engine::components::ComponentName &component) const noexcept
//     {
//         std::size_t seed = 0;
//         engine::utils::hash_combine (member1);
//         engine::utils::hash_combine (member2);
//         ...
//         engine::utils::hash_combine (memberN);  
//         // *each public member should have the std::hash<T>{} specialization 
//         return seed;
//     }
// };
// If you want to generate hash class for the component you should add the
// static constexpr bool kGenerateHash = true; member to the component
// It will modify the engine\components\generated\components-hash.hpp file


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
        std::string name = "None";
        std::string tag;
        entt::entity parent = entt::null;
        std::vector<entt::entity> children;
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

    // TODO:
    // add casts_shadow boolean so that we can have lights that don't cast shadows
    // add shadow_bias
    // add shadow_normal_bias
    // add shadow_blur
    // add shadow_blur_radius
    // add shadow_blur_sigma

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

    // TODO:
    // Add support for multiple particle materials
    // PBR (probably?)
    // Add GPU support

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
        core::math::vec2 position_radius;

        // The velocity yaw range is a vector with the following format: (x, y, z, w) = (min_yaw, min_pitch, max_yaw, max_pitch)
        // range in which the yaw and pitch will be randomized to rotate the base velocity vector
        core::math::vec4 velocity_yaw_pitch_range;
        // range in which the velocity will be randomized
        core::math::vec2 velocity_radius;

        // Base HDR color of the generated particle
        core::math::vec4 base_diffuse_color;
        // Basically works like this: base_diffuse_color + (random_vec4(-1, 1) * diffuse_variation)
        core::math::vec4 diffuse_variation;

        // range in which the particle life span will be randomized
        core::math::vec2 particle_lifespan_range;

        // range in which the start size will be randomized
        core::math::vec2 begin_size_range;
        core::math::vec2 end_size_range;

        // range in which the mass will be randomized
        // mass is used to calculate the drag force
        // TODO: Add a drag force field
        core::math::vec2 mass_range;

        core::math::vec2 thickness_range;

        float emit_rate;

        core::math::vec2 rotation_range;

        core::math::vec2 rotation_speed_range;

        core::math::vec3 particle_acceleration;

        // Maximum amount of particles that can be alive at the same time for this particle emitter
        uint32_t maximum_amount_of_particles;

        bool freeze = false;

    private:
        friend class engine::render::_particle_detail::ParticleRenderSystem;
        struct Particle
        {
            core::math::vec3 position;
            core::math::vec3 velocity;
            // e.g. gravity, wind, etc.
            // better to do with force fields, but this is a quick and dirty solution
            core::math::vec3 acceleration;
            core::math::vec4 color;
            float begin_size;
            float end_size;
            float life_begin;
            float life_end;
            float rotation;
            float rotation_speed;
            float thickness;
        };
        std::vector<Particle> particles;
        // Counts up to 1 second and then resets
        // Used to properly emit particles
        // last_second_count += dt
        // if last_second_count >= 1.0f: reset last_second_count and particles_last_second_count
        // if particles_last_second_count < last_second_count * emit_rate: emit (last_second_count * emit_rate - particles_last_second_count) particles
        // and add them to particles_last_second_count
        float last_second_count;
        uint32_t particles_last_second_count;
    };
    // UNUSED
    struct ForceField final
    {
        enum class Type : uint8_t
        {
            // Point force field is a sphere that applies a force to all the particles inside it
            // Basically works like this: applied_force += force * (1 - (distance / radius) ^ falloff)
            Point = 0,
            // Directional force field is a plane that applies a force to all the particles in front of it
            // Basically works like this: applied_force += direction * force * (1 - (dot(normal, particle_position) / radius) ^ falloff)
            // where direction = transform.forward
            Directional = 1,
            // OmniDirectional force field is a sphere field that applies force inwards of this sphere from all directions
            // Can be used to simulate the air/water/whatever resistance
            // You can calculate approximate force for the field like this:
            // 0.5 * fluid_density * average_particle_velocity ^ 2 * average_particle_area * fluid_drag_coefficient
            // It is pretty costy to simulate the whole fluid dynamics, so this is a good approximation
            // Basically works like this: applied_force += -normalized(velocity.direction) * force
            OmniDirectional = 2
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

    // UNUSED
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