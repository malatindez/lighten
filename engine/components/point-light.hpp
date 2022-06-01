#pragma once
#include "light-data.hpp"
#include "material.hpp"
#include "math.hpp"
#include <cmath>
namespace engine::components
{
    struct PointLight
    {
        math::vec3 color;
        float ambient_intensity;
        float diffuse_intensity;
        struct Attenuation
        {
            float constant;
            float linear;
            float quadratic;
        } attenuation;

        inline void UpdateColor(Transform const &transform, LightData &light_data, Material const&mat,
                                std::function<bool(math::Intersection &, math::Ray &)> const &find_intersection) const
        {
            math::vec3 const light_direction = transform.position - light_data.point;
            float const ndotl = math::dot(light_data.normal, light_direction);
            if(ndotl < 0)
            {
                return;
            }
            math::Intersection nearest;
            nearest.reset();
            math::Ray ray(transform.position, light_direction);
            find_intersection(nearest, ray);

            float const distance = math::length(light_direction);
            float const attenuation_ = 1.0f /
                                 (attenuation.constant +
                                  attenuation.linear * distance +
                                  attenuation.quadratic * distance * distance);
            
            
            math::vec3 const ambient = mat.albedo * ambient_intensity;
            if (nearest.t < distance)
            {
                light_data.color += color * ambient * attenuation_;
                return;
            }
            
            math::vec3 const diffuse = ndotl * diffuse_intensity * mat.albedo;

            float const u = dot(light_data.normal, math::normalize(light_data.view_dir + light_direction));
            float const specular = mat.specular * static_cast<float>(math::pow(std::max(u, 0.0f), mat.glossiness));

            light_data.color += color * attenuation_ * (ambient + diffuse + specular);
            assert(!std::_Is_nan(light_data.color.r));
        }
    };
} // namespace engine