#pragma once
#include "light-data.hpp"
#include "material.hpp"
#include "math.hpp"
#include "point-light.hpp"
#include <cmath>
namespace engine::components
{
    struct SpotLight : public PointLight
    {
        math::vec3 direction; // should be normalized
        float cut_off;

        inline void UpdateColor(
            Transform const &transform, LightData &light_data, Material const &mat,
            std::function<bool(math::Intersection &, math::Ray &, Transform &)> const
                &find_intersection_transform) const
        {
            math::vec3 const L = normalize(light_data.point - transform.position);
            if (dot(L, direction) < cut_off)
            {
                return;
            }
            PointLight::UpdateColor(transform, light_data, mat,
                                    find_intersection_transform);
        }
    };
} // namespace engine::components
