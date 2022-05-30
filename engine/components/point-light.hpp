#pragma once
#include "math.hpp"
#include "light-data.hpp"
#include <cmath>
namespace engine::components
{
    struct PointLight
    {
        math::vec3 color;
        float ambient_intensity;
        float diffuse_intensity;
        float specular_intensity;
        struct Attenuation
        {
            float constant;
            float linear;
            float quadratic;
        } attenuation;

        
        inline void UpdateColor(Transform const &transform, LightData &light_data) const
        {
            math::vec3 const light_direction = - light_data.point + transform.position;
            float const t = math::dot(light_data.normal, light_direction);
            light_data.color += color * ambient_intensity;
            if (t < 0)
            {
                return;
            }
            float distance = math::length(light_direction);
            float attenuation_ = 1.0f /
                                (attenuation.constant +
                                 attenuation.linear * distance +
                                 attenuation.quadratic * distance * distance);

            math::vec3 const reflect_dir = math::reflect_normal(light_data.normal, light_direction);
            
            float diffuse = diffuse_intensity * t;
            float u = dot(light_data.view_dir, reflect_dir);
            float specular = specular_intensity* static_cast<float>(math::pow(std::max(u, 0.0f), 32));
            
            light_data.color += color * attenuation_ * (ambient_intensity + diffuse + specular);
            
        }
    };
} // namespace engine