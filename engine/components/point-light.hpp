#pragma once
#include "core/math.hpp"
#include "render/light-data.hpp"
#include "render/material.hpp"
#include "render/light-common.hpp"
namespace engine::components
{
    struct PointLight
    {
        core::math::vec3 color;
        struct Attenuation
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
            distance /= std::cbrtf(length(transform.scale));
            
            float attenuation = 1.0f / (this->attenuation.constant + this->attenuation.linear * distance + this->attenuation.quadratic * (distance * distance));    

            //float const diffuse = (1 - ) * (1 - mat.metalness)
            float spec = std::max(dot(light_data.normal, H), 0.0f);
            
            light_data.color += ndotl * color * attenuation * (ndotl * mat.albedo + std::powf(spec, mat.glossiness) * mat.specular);
        }
    };
} // namespace engine::components