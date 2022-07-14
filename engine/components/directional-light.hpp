#pragma once
#include "core/math.hpp"
#include "core/math/intersection.hpp"
#include "core/math/ray.hpp"
#include "render/light-common.hpp"
#include "render/light-data.hpp"
#include "render/material.hpp"
#include <functional>
#include <math.h>
namespace engine::components
{
    struct DirectionalLight
    {
        core::math::vec3 direction;
        core::math::vec3 color;
        float solid_angle = 2.0f * float(std::numbers::pi);
        float power;

        inline core::math::vec3 Illuminate(render::LightData &light_data, render::Material const &mat) const
        {
            core::math::vec3 L = -direction;
            if (float ndotl = dot(light_data.normal, L);
                ndotl <= 0)
            {
                return core::math::vec3{0, 0, 0};
            }
            return render::Illuminate(L, L, light_data, mat, solid_angle / (2.0f * float(std::numbers::pi)), color, power);
        }
    };
} // namespace engine::components