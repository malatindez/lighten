#pragma once
#include "light-data.hpp"
#include "material.hpp"
#include "math.hpp"
#include <cmath>
namespace engine::components
{
    struct PointLight
    {
        math::vec3 color;
        float R;

        inline void UpdateColor(Transform const &transform, LightData &light_data, Material const&mat,
                                std::function<bool(math::Intersection &, math::Ray &)> const &find_intersection) const
        {
           math::vec3 L = transform.position - light_data.point;
           math::vec3 H = math::normalize(light_data.view_dir + L);
           float distance = length(L);
           L = normalize(L);
           float ndotl = dot(light_data.normal, L);
           if(ndotl < 0)
            {
                return;
            }
           float const specular = pow(dot(light_data.normal, H), mat.glossiness) * mat.specular;
           distance /= R;
           distance *= distance;
           light_data.color += color * ndotl * (ndotl * mat.albedo / distance + specular);
        }
    };
} // namespace engine