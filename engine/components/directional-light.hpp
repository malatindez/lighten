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
        float R = 5.0f; // if intersection was found then we multiply light color by 1
                        // / std::pow(max(1, sqrt(R - nearest.t)), 2)

        inline void
        UpdateColor(LightData &light_data, Material const &mat,
                    std::function<bool(math::Intersection &, math::Ray &)> const
                        &find_intersection) const
        {
            float ndotl = dot(light_data.normal, -direction);
            if (ndotl <= 0)
            {
                return;
            }
            math::Intersection nearest;
            nearest.reset();
            math::Ray ray(light_data.point - direction * 0.01f, -direction);
            find_intersection(nearest, ray);
            float specular =
                pow(dot(light_data.normal, light_data.view_dir), mat.glossiness) *
                mat.specular;
            float t = 1;
            if (nearest.exists())
            {
                t = std::max(1.0f, R - nearest.t);
                t = std::sqrt(t);
            }
            light_data.color += color * ndotl * (ndotl * mat.albedo + specular) / t;
        }
    };
} // namespace engine::components