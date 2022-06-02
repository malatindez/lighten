#pragma once

#include "components/camera.hpp"
#include "components/cube.hpp"
#include "components/directional-light.hpp"
#include "components/material.hpp"
#include "components/mesh.hpp"
#include "components/plane.hpp"
#include "components/point-light.hpp"
#include "components/sphere.hpp"
#include "components/spot-light.hpp"
#include "core/bitmap-window.hpp"
#include "core/parallel-executor.hpp"
#include "entt/entt.hpp"
#include "math/ray.hpp"

namespace engine
{
    namespace _sdet // scene_detail
    {
        template <class T>
        using cref = std::reference_wrapper<const T>;
        // vector of tuples containing constant references
        template <class... T>
        using cref_tuple_vec = std::vector<std::tuple<cref<T>...>>;
        // vector of tuples containing constant references skipping the first one
        // skips cref on the first template argument
        // we need it so the entt::entity is not wrapped
        template <class A, class... T>
        using cref_tuple_vec_sf = std::vector<std::tuple<A, cref<T>...>>;
        // _sdet::cref_tuple_vec_sf<entt::entity, components::Plane,
        // components::Transform, components::Material> planes; vs
        // std::vector<std::tuple<entt::entity,
        // std::reference_wrapper<components::Plane>,
        // std::reference_wrapper<components::Transform>,
        // std::reference_wrapper<components::Material>>> planes;
    } // namespace _sdet
    class Scene
    {
    public:
        Scene() = default;
        void UpdateScene() noexcept;

        void Draw(components::Camera const &cam, BitmapWindow &window, ParallelExecutor &executor);

        math::Intersection FindIntersection(math::Ray const &ray);
        // heavy function.
        // should be updated whenever any of the rendered objects are added or deleted
        void OnRegistryUpdate();
        inline bool FindIntersection(math::Intersection &intersection, math::Ray &ray);
        [[nodiscard]] std::optional<entt::entity> GetIntersectedEntity(math::Intersection &intersection, math::Ray &ray);
        bool update_scene{true};

        entt::registry registry;

        _sdet::cref_tuple_vec_sf<entt::entity, components::Plane, components::Transform, components::Material> planes;
        _sdet::cref_tuple_vec_sf<entt::entity, components::Transform, components::Material> spheres;
        _sdet::cref_tuple_vec_sf<entt::entity, components::Transform, components::Material> cubes;
        _sdet::cref_tuple_vec_sf<entt::entity, components::Mesh, components::Transform, components::Material> meshes;
        _sdet::cref_tuple_vec_sf<entt::entity, components::DirectionalLight> directional_lights;
        _sdet::cref_tuple_vec_sf<entt::entity, components::PointLight, components::Transform> point_lights;
        _sdet::cref_tuple_vec_sf<entt::entity, components::SpotLight, components::Transform> spot_lights;
        inline bool GetIntersectedMaterial(math::Intersection &intersection, math::Ray &ray, components::Material &mat);
        inline bool GetIntersectedTransform(math::Intersection &intersection, math::Ray &ray, components::Transform &t);
    };
} // namespace engine