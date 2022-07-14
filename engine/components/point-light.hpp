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
            float distance = length(sphereRelPos);
            core::math::vec3 const &N = light_data.normal;
            core::math::vec3 L = normalize(sphereRelPos);
            float radius = length(transform.scale) / sqrtf(3);
            distance = std::max(distance, radius);
            float cosa = sqrtf(1.0f - radius * radius / distance / distance);
            float attenuation = (1.0f - cosa); // solid_angle / 2.0f * float(std::numbers::pi)

            bool intersects = false;
            core::math::vec3 const R = core::math::reflect_normal_safe(N, -normalize(light_data.view_dir));
            core::math::vec3 D = render::approximateClosestSphereDir(intersects, R, cosa, sphereRelPos, L, distance, radius);
            float ndotl = dot(N, L);
            render::clampDirToHorizon(D, ndotl, N, 0.005f);

            return render::Illuminate(L, D, light_data, mat, attenuation, color, power);
        }
    };
} // namespace engine::components