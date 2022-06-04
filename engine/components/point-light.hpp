#pragma once
#include "render/light-data.hpp"
#include "render/material.hpp"
#include "math.hpp"
#include <cmath>
namespace engine::components
{
    struct PointLight
    {
        math::vec3 color;
        float R;
        
        inline bool Illuminable(Transform const &transform, render::LightData const&light_data) const noexcept
        {
            math::vec3 L = transform.position - light_data.point;
            math::vec3 const H = math::normalize(light_data.ray.origin() - light_data.point);
            float distance = length(L);
            L = normalize(L);
            float ndotl = dot(light_data.normal, L);
            if (ndotl < 0)
            {
                return false;
            }
            return true;
        }
        inline void Illuminate(Transform const &transform, render::LightData &light_data, render::Material const &mat) const
        {
            math::vec3 L = transform.position - light_data.point;
            math::vec3 const H = math::normalize(light_data.ray.origin() - light_data.point);
            float distance = length(L);
            L = normalize(L);
            float ndotl = dot(light_data.normal, L);
            distance /= R;
            distance *= distance;
            light_data.color += ndotl * color * (ndotl * mat.albedo / distance + pow(dot(light_data.normal, H), mat.glossiness) * mat.specular);
        }
    };
} // namespace engine::components