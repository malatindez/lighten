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
        float R;

        inline void UpdateColor(Transform const &transform, LightData &light_data, Material const&mat,
                                std::function<bool(math::Intersection &, math::Ray &, Transform &)> const &find_intersection_transform) const
        {
           math::vec3 L = transform.position - light_data.point;
           math::vec3 const H = math::normalize(light_data.view_dir + L);
           float distance = length(L);
           L = normalize(L);
           float ndotl = dot(light_data.normal, L);
           if(ndotl < 0)
            {
                return;
            }
            math::Intersection nearest;
            nearest.reset();
            math::Ray ray(light_data.point + L * 0.001f, + L);
            Transform t;
            find_intersection_transform(nearest, ray, t);
            float modifier = ndotl * 0.25f * ndotl;
            if(!(nearest.exists() && transform.position != t.position) && distance >= nearest.t) {
                float const specular = pow(dot(light_data.normal, H), mat.glossiness) * mat.specular;
                modifier = ndotl;
                light_data.color += modifier * color * specular;
            }

           distance /= R;
           distance *= distance;
           light_data.color += modifier * color * (ndotl * mat.albedo / distance);
        }
    };
} // namespace engine