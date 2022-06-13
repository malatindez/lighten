#pragma once
#include "core/math.hpp"
#include "point-light.hpp"
#include "render/light-data.hpp"
#include "render/material.hpp"
#include <math.h>
namespace engine::components
{
    struct SpotLight : public PointLight
    {
        core::math::vec3 direction; // should be normalized
        float cut_off;
        inline bool Illuminable(Transform const &transform, render::LightData const &light_data) const noexcept
        {
            core::math::vec3 const L = normalize(light_data.point - transform.position);
            if (dot(L, direction) < cut_off)
            {
                return false;
            }
            return PointLight::Illuminable(transform, light_data);
        }
    };
} // namespace engine::components
