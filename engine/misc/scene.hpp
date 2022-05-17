#pragma once

#include "core/bitmap-window.hpp"
#include "math/ray.hpp"
#include "components/sphere.hpp"
#include "components/directional-light.hpp"
#include "components/point-light.hpp"
#include "components/spot-light.hpp"
#include "camera.hpp"

namespace engine
{
    class Scene
    {
    public:
        Scene() = default;
        void UpdateScene() noexcept;

        void Draw(Camera &cam, BitmapWindow &window);

        math::Intersection FindIntersection(math::Ray const& ray);
        
        bool update_scene{true};
        entt::registry registry;
    };
} // namespace engine