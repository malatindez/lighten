#pragma once

#include "core/bitmap-window.hpp"
#include "math/ray.hpp"
#include "math/sphere.hpp"

namespace engine
{
    class Scene
    {
    public:
        Scene() = default;
        void UpdateScene() noexcept;

        void Draw(BitmapWindow &window);

        math::Sphere sphere{math::vec3{0, 0, 0}, 1};

    private:
        bool update_scene_{true};
    };
} // namespace engine