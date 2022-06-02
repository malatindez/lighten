#pragma once
#include "mesh.hpp"
namespace engine::components
{
    inline bool Mesh::CheckTriangleIntersection(std::vector<render::Vertex>::const_iterator &it,
                                                math::Intersection &i,
                                                math::Ray const &ray) noexcept
    {
        math::vec3 const &p0 = (it++)->position;
        math::vec3 const &p1 = (it++)->position;
        math::vec3 const &p2 = (it)->position;
        math::vec3 const &normal = (it++)->normal;

        float ndotdir = math::dot(normal, ray.direction());
        if (fabs(ndotdir) < 1e-6f)
        {
            return false;
        }
        float t = math::dot(p0 - ray.origin(), normal) / ndotdir;
        if (t > i.t || t < 0)
        {
            return false;
        }
        math::vec3 P = ray.PointAtParameter(t);
        math::vec3 c;
        math::vec3 edge0 = p1 - p0;
        math::vec3 vp0 = P - p0;
        c = math::cross(edge0, vp0);
        if (math::dot(normal, c) < 0.0f)
        {
            return false;
        }
        math::vec3 edge1 = p2 - p1;
        math::vec3 vp1 = P - p1;
        c = math::cross(edge1, vp1);
        if (math::dot(normal, c) < 0.0f)
        {
            return false;
        }
        math::vec3 edge2 = p0 - p2;
        math::vec3 vp2 = P - p2;
        c = math::cross(edge2, vp2);
        if (math::dot(normal, c) < 0.0f)
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