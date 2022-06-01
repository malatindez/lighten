#pragma once
#include "light-data.hpp"
#include "material.hpp"
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

        inline void UpdateColor(LightData &light_data, Material const&mat,
                                std::function<bool(math::Intersection &, math::Ray &)> const &find_intersection)
        {
            assert(math::almost_equal(length(direction), 1.0f));
            float const ndotl = math::dot(light_data.normal, direction);
            light_data.color += color * ambient_intensity;
            if(ndotl < 0.0f)
            {
                return;
            }
            
            float const u = dot(light_data.normal, light_data.view_dir);
            float const specular = mat.specular * static_cast<float>(math::pow(std::max(u, 0.0f), mat.glossiness));

            light_data.color += color * (std::max(ndotl, 0.0f) * mat.albedo + specular);
            assert(!std::_Is_nan(light_data.color.r));
        }
    };
} // namespace engine