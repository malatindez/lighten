#pragma once
#include "math.hpp"
#include "light-data.hpp"
#include <cmath>
namespace engine::components
{
    struct DirectionalLight
    {
        math::vec3 direction;
        
        math::vec3 color;
        float ambient_intensity;
        float diffuse_intensity;
        float specular_intensity;

        
        
        inline void UpdateColor(LightData &light_data)
        {
            assert(math::almost_equal(length(direction), 1.0f));
            float const t = math::dot(light_data.normal, -direction);
            light_data.color += color * ambient_intensity;
            if (t < 0)
            {
                return;
            }
            math::vec3 const reflect_dir = 2.0f * 
                    math::dot(light_data.normal, -direction) *
                    (light_data.normal + direction);

            light_data.color += color *
                     (
                      diffuse_intensity * t
                      + specular_intensity * std::pow(std::max(dot(light_data.view_dir, reflect_dir), 0.0f), 32)
                     );
        }
    };
} // namespace engine