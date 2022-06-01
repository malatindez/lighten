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
        template <class... T>
        using cref_tuple_vec = std::vector<std::tuple<cref<T>...>>;
        template <class T>
        using cref_vec = std::vector<cref<T>>;
    }
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
        bool FindIntersection(math::Intersection &intersection, math::Ray &ray);
        bool FindIntersectionMaterial(math::Intersection &intersection, math::Ray &ray, components::Material &mat);
        bool update_scene{true};

        entt::registry registry;

        _sdet::cref_tuple_vec<components::Plane, components::Transform, components::Material> planes;
        _sdet::cref_tuple_vec<components::Transform, components::Material> spheres;
        _sdet::cref_tuple_vec<components::Transform, components::Material> cubes;
        _sdet::cref_tuple_vec<components::Mesh, components::Transform, components::Material> meshes;
        _sdet::cref_vec<components::DirectionalLight> directional_lights;
        _sdet::cref_tuple_vec<components::PointLight, components::Transform> point_lights;
        _sdet::cref_tuple_vec<components::SpotLight, components::Transform> spot_lights;
    };
} // namespace engine