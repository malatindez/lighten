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
        float power;
        inline bool Illuminable(Transform const &transform, render::LightData const &light_data) const noexcept
        {
            core::math::vec3 L = normalize(transform.position - light_data.point);
            float ndotl = dot(light_data.normal, L);
            if (ndotl <= 0)
            {
                return false;
            }
            return true;
        }
        inline core::math::vec3 Illuminate(Transform const &transform, render::LightData &light_data, render::Material const &mat) const
        {
            core::math::vec3 sphereRelPos = transform.position - light_data.point;
            float const distance = length(sphereRelPos);
            core::math::vec3 const &N = light_data.normal;
            core::math::vec3 L = normalize(sphereRelPos);
            float const radius = length(transform.scale);
            
            float solid_angle = float(2.0f * std::numbers::pi);
            if(distance > radius) 
            {
                solid_angle = 2.0f * static_cast<float>(std::numbers::pi) * (1.0f - sqrtf(distance * distance - radius * radius) / distance);
            }
            
            bool intersects = false;
            core::math::vec3 const R = core::math::reflect_normal_safe(-L, N);
            core::math::vec3 D = render::approximateClosestSphereDir(intersects, R,sqrtf(1 -  radius/distance * radius/distance), sphereRelPos, L, distance, radius);
            float ndotl = std::min(dot(N, L), 0.0f);
            render::clampDirToHorizon(D, ndotl, N, 0.0f);

            return render::Illuminate(D, light_data, mat, solid_angle, color, power);
        }
    };
} // namespace engine::components