#pragma once
#include "core/math.hpp"
#include "render/light-common.hpp"
#include "render/light-data.hpp"
#include "render/material.hpp"
namespace engine::components
{
    struct PointLight
    {
        core::math::vec3 color;
        inline bool Illuminable(Transform const &transform, render::LightData const &light_data) const noexcept
        {
            core::math::vec3 L = normalize(transform.position - light_data.point);
            float ndotl = dot(light_data.normal, L);
            if (ndotl < 0)
            {
                return false;
            }
            return true;
        }
        inline core::math::vec3 Illuminate(Transform const &transform, render::LightData &light_data, render::Material const &mat) const
        {
            core::math::vec3 L = transform.position - light_data.point;
            float const distance = length(L);
            L = normalize(L);
            float const radius = length(transform.scale);
            
            float solid_angle = float(2.0f * std::numbers::pi);
            if(distance > radius) 
            {
                solid_angle = 2.0f * static_cast<float>(std::numbers::pi) * (1.0f - sqrtf(distance * distance - radius * radius) / distance);
            }

            return render::Illuminate(L, light_data, mat, solid_angle, color);
        }
    };
} // namespace engine::components