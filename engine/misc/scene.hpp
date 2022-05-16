#pragma once

#include "core/bitmap-window.hpp"
#include "math/ray.hpp"
#include "math/sphere.hpp"
#include "components/directional-light.hpp"
#include "components/point-light.hpp"
#include "components/spot-light.hpp"

namespace engine
{
    class Scene
    {
    public:
        Scene() = default;
        void UpdateScene() noexcept;

        void Draw(BitmapWindow &window);

        bool FindIntersection()
        {
            
        }

        math::Sphere sphere{math::vec3{0, 0, 0}, 1};
        
        std::vector<components::SpotLight> spot_lights;
        std::vector<components::PointLight> point_lights;
        std::vector<components::DirectionalLight> directional_lights;

        bool update_scene{true};
    };
} // namespace engine