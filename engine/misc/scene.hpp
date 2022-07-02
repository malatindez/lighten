#pragma once

#include "components/camera.hpp"
#include "components/directional-light.hpp"
#include "components/point-light.hpp"
#include "components/scene-mesh.hpp"
#include "components/scene-sphere.hpp"
#include "components/spot-light.hpp"
#include "core/bitmap-window.hpp"
#include "core/math/ray.hpp"
#include "core/parallel-executor.hpp"
#include "core/utils.hpp"
#include "entt/entt.hpp"
#include "render/floor.hpp"
namespace engine
{
    class Scene
    {
    public:
        Scene() = default;
        void UpdateScene() noexcept;
        using SphereGroup = entt::basic_group<entt::entity, entt::type_list<engine::components::SceneSphere>, entt::type_list<engine::components::Transform>, entt::type_list<>>;
        using MeshGroup = entt::basic_group<entt::entity, entt::type_list<engine::components::SceneMesh>, entt::type_list<engine::components::Transform>, entt::type_list<>>;
        using DirectionalLightView = entt::basic_view<entt::entity, entt::type_list<engine::components::DirectionalLight>, entt::type_list<>, void>;
        using PointLightGroup = entt::basic_group<entt::entity, entt::type_list<engine::components::PointLight>, entt::type_list<engine::components::Transform>, entt::type_list<>>;
        using SpotLightGroup = entt::basic_group<entt::entity, entt::type_list<engine::components::SpotLight>, entt::type_list<engine::components::Transform>, entt::type_list<>>;

        using IntersectionCallbackFn = std::function<bool(entt::entity, engine::components::Transform const &, render::Material const &)>;

        void Draw(components::Camera const &cam, core::BitmapWindow &window, core::ParallelExecutor &executor);

        [[nodiscard]] std::optional<entt::entity> GetIntersectedEntity(core::math::Intersection &intersection, core::math::Ray &ray);

        std::optional<entt::entity> GetIntersectedEntityIf(
            core::math::Intersection &intersection, core::math::Ray &ray,
            std::function<bool(entt::entity, components::Transform const &, render::Material const &)> const &func);

        bool FindIntersection(SphereGroup &spheres, MeshGroup &meshes, core::math::Intersection &intersection, core::math::Ray &ray) const noexcept;

        bool FindIntersectionIf(SphereGroup &spheres, MeshGroup &meshes, core::math::Intersection &intersection, core::math::Ray &ray, IntersectionCallbackFn const &func) const noexcept;

        core::math::vec3 CalculatePointColor(SphereGroup &spheres,
                                             MeshGroup &meshes,
                                             DirectionalLightView &directional_lights,
                                             PointLightGroup &point_lights,
                                             SpotLightGroup &spot_lights,
                                             core::math::Ray const &ray, core::math::Intersection const &nearest,
                                             engine::render::Material const &mat,
                                             int depth = 0);

        core::math::vec3  Illuminate(SphereGroup &spheres,
                               MeshGroup &meshes,
                               DirectionalLightView &directional_lights,
                               PointLightGroup &point_lights,
                               SpotLightGroup &spot_lights,
                               render::Material const &mat,
                               render::LightData &ld);
        core::math::vec3 CalculatePointGIAmbient(SphereGroup &spheres,
                                                 MeshGroup &meshes,
                                                 DirectionalLightView &directional_lights,
                                                 PointLightGroup &point_lights,
                                                 SpotLightGroup &spot_lights,
                                                 core::math::Intersection const &nearest,
                                                 int depth = 0);

        bool update_scene{true};
        bool reflections_on{true};
        bool global_illumination_on{false};
        int hemisphere_ray_count = 200;
        float exposure = 2.0f;
        float gamma = 2.2f;
        float reflection_roughness_threshold = 0.1f;
        int max_ray_depth = 4;
        core::math::vec3 ambient = core::math::vec3{ 0 };// render::UIntToRGB(0x0B1026) / 0xff;
        entt::registry registry;
        render::Floor floor;
    };
} // namespace engine