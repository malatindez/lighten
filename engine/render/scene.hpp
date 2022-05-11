#pragma once

#include "math/sphere.hpp"
#include "core/bitmap-window.hpp"
#include "math/ray.hpp"

namespace engine
{
    class Scene
    {
    public:
        Scene() {}
        void UpdateScene() noexcept;

        void Draw(BitmapWindow &window);

        math::Sphere sphere{math::vec3{0, 0, 0}, 1};
        
    private:
        bool update_scene_{true};
    };
}