#pragma once

#include "math/sphere.hpp"
#include "misc/bitmap-window.hpp"
namespace engine
{
    class Scene
    {
    public:
        Scene() {}
        void UpdateScene() noexcept;

        void Draw(BitmapWindow &window);

        math::Sphere sphere{{0, 0, 0}, 1};

    private:
        bool update_scene_{true};
    };
}