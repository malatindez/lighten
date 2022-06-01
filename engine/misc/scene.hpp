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
    class Scene
    {
    public:
        Scene() = default;
        void UpdateScene() noexcept;

        void Draw(components::Camera const &cam, BitmapWindow &window, ParallelExecutor &executor);

        math::Intersection FindIntersection(math::Ray const &ray);

        bool update_scene{true};

        entt::registry registry;
    };
} // namespace engine