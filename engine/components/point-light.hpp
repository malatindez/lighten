#pragma once
#include "render/light-data.hpp"
#include "render/material.hpp"
#include "math.hpp"
#include <math.h>
namespace engine::components
{
    struct PointLight
    {
        core::math::vec3 color;
        float R;
        
        inline bool Illuminable(Transform const &transform, render::LightData const&light_data) const noexcept
        {
            core::math::vec3 const L = normalize(transform.position - light_data.point);
            float ndotl = dot(light_data.normal, L);
            if (ndotl < 0)
            {
                return false;
            }
            return true;
        }
        inline void Illuminate(Transform const &transform, render::LightData &light_data, render::Material const &mat) const
        {
            core::math::vec3 L = transform.position - light_data.point;
            core::math::vec3 const H = core::math::normalize(light_data.ray.origin() - light_data.point);
            float distance = length(L);
            L = normalize(L);
            float ndotl = dot(light_data.normal, L);
            distance /= R * std::cbrt(length(transform.scale));
            distance *= distance;
            light_data.color += ndotl * color * (ndotl * mat.albedo / distance + pow(dot(light_data.normal, H), mat.glossiness) * mat.specular);
        }
    };
} // namespace engine::components