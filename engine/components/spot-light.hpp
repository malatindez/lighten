#pragma once
#include "light-data.hpp"
#include "point-light.hpp"
#include "material.hpp"
#include "math.hpp"
#include <cmath>
namespace engine::components
{
    struct SpotLight : public PointLight
    {
        math::vec3 direction; // should be normalized
        float cut_off;

        inline void UpdateColor(Transform const &transform, LightData &light_data, Material const&mat,
                                std::function<bool(math::Intersection &, math::Ray &)> const &find_intersection) const
        {
            assert(math::almost_equal(length(direction), 1.0f));

            math::vec3 const light_direction = light_data.point - transform.position;

            auto k = dot(normalize(light_direction), direction);
            if (k < cut_off)
            {
                float const distance = math::length(light_direction);
                float const attenuation = 1.0f /
                    (this->attenuation.constant +
                        this->attenuation.linear * distance +
                        this->attenuation.quadratic * distance * distance);
                light_data.color += this->color * attenuation * ambient_intensity;
                return;
            }
            PointLight::UpdateColor(transform, light_data, mat, find_intersection);
        }
    };
} // namespace engine
