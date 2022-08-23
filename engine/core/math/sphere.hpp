#pragma once
#include "ray.hpp"
namespace engine::core::math
{
    struct Sphere
    {
        static constexpr float Hit(core::math::Ray const &r, core::math::vec3 const &center, float radius) noexcept
        {
            const core::math::vec3 oc = r.origin() - center;
            const float a = core::math::dot(r.direction(), r.direction());
            const float b = core::math::dot(oc, r.direction());
            const float c = core::math::dot(oc, oc) - radius;
            const float discriminant = b * b - a * c;
            if (discriminant < 0)
            {
                return -1.0f;
            }
            const float d = core::math::detail::sqrt(discriminant);
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

        bool CheckIntersection(core::math::Intersection &i, core::math::Ray const &ray) const
        {
            float t = Sphere::Hit(ray, position, radius);

            if (t > i.t || t < 0)
            {
                return false;
            }
            i.t = t;
            i.point = ray.PointAtParameter(t);
            i.normal = normalize(i.point - position);
            i.normal = i.normal * (core::math::length(ray.origin()) < 1 ? -1 : 1);
            return true;
        }

        core::math::vec3 position { 0 };
        float radius { 1 };
    };
}