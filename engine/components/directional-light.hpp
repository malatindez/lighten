#pragma once
#include "light-data.hpp"
#include "math.hpp"
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
            math::vec3 const reflect_dir = math::reflect_normal_safe(light_data.normal, -direction);
            assert(!std::_Is_nan(reflect_dir.x));

            light_data.color += color *
                                (diffuse_intensity * t + specular_intensity * math::pow(std::max(dot(light_data.view_dir, reflect_dir), 0.0f), 32));
            assert(!std::_Is_nan(light_data.color.r));
        }
    };
} // namespace engine