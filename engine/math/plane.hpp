#pragma once
#include "math/intersection.hpp"
#include "math/ray.hpp"
namespace engine::core::math
{
    struct Plane
    {
        bool CheckIntersection(core::math::Intersection &i, core::math::Ray const &ray) const
        {
            float denom = core::math::dot(normal, ray.direction());
            if (abs(denom) <= 1e-6f)
            {
                return false;
            }
            float t = core::math::dot(-ray.origin(), normal) / denom;
            if (t > i.t || t <= 0)
            {
                return false;
            }
            i.t = t;
            // reverse normal if the ray is on the opposite side of the plane
            i.normal = normal * (denom > 0 ? -1 : 1);
            i.point = ray.PointAtParameter(t);
            return true;
        }
        void update_plane(core::math::vec3 const &first, core::math::vec3 const &second)
        {
            core::math::vec3 temp = -core::math::cross(first, second);
            if (core::math::length(temp) == 0)
            {
                throw std::invalid_argument("Input vectors can't be collinear!");
            }
            v = first;
            w = second;
            normal = core::math::normalize(temp);
        }

        core::math::vec3 v;
        core::math::vec3 w;
        core::math::vec3 normal;
    };
}