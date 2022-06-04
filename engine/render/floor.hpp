#pragma once
#include "render/material.hpp"
namespace engine
{
    struct Floor
    {
        float Hit(math::Ray const &r, math::vec3 center, float radius) noexcept
        {
            return -1;
        }

        bool CheckIntersection(math::Intersection &i, math::Ray const &ray) const
        {
            float denom = math::dot(normal, ray.direction());
            if (abs(denom) <= 1e-6f)
            {
                return false;
            }
            math::vec3 a = transform.position - ray.origin();
            float t = math::dot(a, normal) / denom;
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

        void update_plane(math::vec3 const &first, math::vec3 const &second)
        {
            math::vec3 temp = -math::cross(first, second);
            if (math::length(temp) == 0)
            {
                throw std::invalid_argument("Input vectors can't be collinear!");
            }
            v = first;
            w = second;
            normal = math::normalize(temp);
        }

        render::Material material;
        components::Transform transform;
        math::vec3 v;
        math::vec3 w;
        math::vec3 normal;
    };
}