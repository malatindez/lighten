#pragma once
#include "math.hpp"
#include "render/light-data.hpp"
#include "render/material.hpp"
#include <math.h>
namespace engine::components
{
    struct PointLight
    {
        core::math::vec3 color;
        struct attenuation
        {
            float constant;
            float linear;
            float quadratic;
        } attenuation;

        inline bool Illuminable(Transform const &transform, render::LightData const &light_data) const noexcept
        {
            core::math::vec3 L = normalize(transform.position - light_data.point);
            float ndotl = dot(light_data.normal, L);
            if (ndotl < 0)
            {
                return false;
            }
            return true;
        }
        inline void Illuminate(Transform const &transform, render::LightData &light_data, render::Material const &mat) const
        {
            core::math::vec3 L = transform.position - light_data.point;
            float distance = length(L);
            L = normalize(L);
            core::math::vec3 const H = core::math::normalize(normalize(L) + light_data.view_dir);
            float ndotl = dot(light_data.normal, L);
            distance /= std::cbrt(length(transform.scale));
            
            float a = 1.0f / (attenuation.constant + attenuation.linear * distance + 
    		    attenuation.quadratic * (distance * distance));    

            float spec = std::max(dot(light_data.normal, H), 0.0f);
            light_data.color += ndotl * color * a * (ndotl * mat.albedo + pow(spec, mat.glossiness) * mat.specular);
        }
    };
} // namespace engine::components