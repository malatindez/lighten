#pragma once
#include "core/math.hpp"
#include "core/math/intersection.hpp"
#include "core/math/ray.hpp"
#include "render/light-data.hpp"
#include "render/material.hpp"
#include "render/light-common.hpp"
#include <functional>
#include <math.h>
namespace engine::components
{
    struct DirectionalLight
    {
        core::math::vec3 direction;
        core::math::vec3 color;
        float solid_angle = 2.0f * static_cast<float>(std::numbers::pi);

        inline void Illuminate(render::LightData &light_data, render::Material const &mat) const
        {
            core::math::vec3 L = -direction;
            float ndotl = dot(light_data.normal, L);
            if (ndotl <= 0)
            {
                return;
            }
            light_data.color += render::Illuminate(L, light_data, mat, solid_angle, color);
        }
    };
} // namespace engine::components