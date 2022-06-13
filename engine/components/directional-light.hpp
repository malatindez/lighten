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

        inline void Illuminate(render::LightData &light_data, render::Material const &mat) const
        {
            float ndotl = dot(light_data.normal, -direction);
            if (ndotl <= 0)
            {
                return;
            }
            float specular = std::powf(dot(light_data.normal, light_data.view_dir), mat.glossiness) * mat.specular;
            light_data.color += color * ndotl * (ndotl * mat.albedo + specular);
        }
    };
} // namespace engine::components