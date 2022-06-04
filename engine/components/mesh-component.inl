#pragma once
#include "mesh-component.hpp"
namespace engine::components
{
    inline bool MeshComponent::CheckTriangleIntersection(
        core::math::vec3 const &p0,
        core::math::vec3 const &p1,
        core::math::vec3 const &p2,
        core::math::vec3 const &normal,
        core::math::Intersection &i,
        core::math::Ray const &ray) noexcept
    {

        float ndotdir = core::math::dot(normal, ray.direction());
        if (fabs(ndotdir) < 1e-6f)
        {
            return false;
        }
        float d = -dot(normal, p0);
        float t = -(core::math::dot(ray.origin(), normal) + d) / ndotdir;
        if (t > i.t || t < 0)
        {
            return false;
        }
        core::math::vec3 P = ray.PointAtParameter(t);
        core::math::vec3 c;
        core::math::vec3 edge0 = p1 - p0;
        core::math::vec3 vp0 = P - p0;
        c = core::math::cross(edge0, vp0);
        if (core::math::dot(normal, c) < 0.0f)
        {
            return false;
        }
        core::math::vec3 edge1 = p2 - p1;
        core::math::vec3 vp1 = P - p1;
        c = core::math::cross(edge1, vp1);
        if (core::math::dot(normal, c) < 0.0f)
        {
            return false;
        }
        core::math::vec3 edge2 = p0 - p2;
        core::math::vec3 vp2 = P - p2;
        c = core::math::cross(edge2, vp2);
        if (core::math::dot(normal, c) < 0.0f)
        {
            return false;
        }
        i.point = P;
        // reverse normal if the ray is on the opposite side of the triangle
        i.normal = normal * (ndotdir > 0 ? -1 : 1);
        i.t = t;
        return true;
    }
}