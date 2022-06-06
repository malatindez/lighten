#pragma once

#include "components/camera.hpp"
#include "components/directional-light.hpp"
#include "components/mesh-renderer.hpp"
#include "components/point-light.hpp"
#include "components/sphere-renderer.hpp"
#include "components/spot-light.hpp"
#include "core/bitmap-window.hpp"
#include "core/parallel-executor.hpp"
#include "core/utils.hpp"
#include "entt/entt.hpp"
#include "math/ray.hpp"
#include "render/floor.hpp"
namespace engine
{
    class Scene
    {
    public:
        Scene() = default;
        void UpdateScene() noexcept;
        using SphereGroup = entt::basic_group<entt::entity, entt::type_list<engine::components::SphereRenderer>, entt::type_list<engine::components::Transform>, entt::type_list<>>;
        using MeshGroup = entt::basic_group<entt::entity, entt::type_list<engine::components::MeshRenderer>, entt::type_list<engine::components::Transform>, entt::type_list<>>;
        using DirectionalLightView = entt::basic_view<entt::entity, entt::type_list<engine::components::DirectionalLight>, entt::type_list<>, void>;
        using PointLightGroup = entt::basic_group<entt::entity, entt::type_list<engine::components::PointLight>, entt::type_list<engine::components::Transform>, entt::type_list<>>;
        using SpotLightGroup = entt::basic_group<entt::entity, entt::type_list<engine::components::SpotLight>, entt::type_list<engine::components::Transform>, entt::type_list<>>;

        using IntersectionCallbackFn = std::function<bool(entt::entity, engine::components::Transform const &, render::Material const &)>;

        void Draw(components::Camera const &cam, core::BitmapWindow &window, core::ParallelExecutor &executor);

        [[nodiscard]] std::optional<entt::entity> GetIntersectedEntity(core::math::Intersection &intersection, core::math::Ray &ray);

        std::optional<entt::entity> GetIntersectedEntityIf(
            core::math::Intersection &intersection, core::math::Ray &ray,
            std::function<bool(entt::entity, components::Transform const &, render::Material const &)> const &func);
        bool update_scene{true};

        bool FindIntersection(SphereGroup &spheres, MeshGroup &meshes, core::math::Intersection &intersection, core::math::Ray &ray) const noexcept;

        bool FindIntersectionIf(SphereGroup &spheres, MeshGroup &meshes, core::math::Intersection &intersection, core::math::Ray &ray, IntersectionCallbackFn const &func) const noexcept;
        
        void Illuminate(SphereGroup &spheres,
                        MeshGroup &meshes,
                        DirectionalLightView &directional_lights,
                        PointLightGroup &point_lights,
                        SpotLightGroup &spot_lights,
                        render::LightData &ld,
                        render::Material &mat) const;

        entt::registry registry;
        render::Floor floor;
    };
} // namespace engine