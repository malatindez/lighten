#pragma once
#include "ray.hpp"
namespace engine::core::math
{
    struct Sphere
    {
        static constexpr float Hit(Ray const &r, vec3 const &center, float radius) noexcept
        {
            const vec3 oc = r.origin() - center;
            const float a = dot(r.direction(), r.direction());
            const float b = dot(oc, r.direction());
            const float c = dot(oc, oc) - radius;
            const float discriminant = b * b - a * c;
            if (discriminant < 0)
            {
                return -1.0f;
            }
            const float d = sqrt(discriminant);
            float rv0 = (-b - d) / a;
            float rv1 = (-b + d) / a;
            if (rv0 > rv1)
            {
                std::swap(rv0, rv1);
            }
            if (rv0 < 0)
            {
                return rv1;
            }
            return rv0;
        }

        bool Intersect(Intersection &i, Ray const &ray) const
        {
            float t = Sphere::Hit(ray, position, radius);

            if (t > i.t || t < 0)
            {
                return false;
            }
            i.t = t;
            i.point = ray.PointAtParameter(t);
            i.normal = normalize(i.point - position);
            i.normal = i.normal * (length(ray.origin()) < 1 ? -1 : 1);
            return true;
        }

        vec3 position{ 0 };
        float radius{ 1 };
    };
}