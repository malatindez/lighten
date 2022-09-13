#pragma once
#include "../math.hpp"
namespace engine::core::math
{
    struct Triangle
    {
        vec3 a;
        vec3 b;
        vec3 c;
        [[nodiscard]] static bool Intersect(vec3 const &p0, vec3 const &p1, vec3 const &p2, vec3 const &normal, float &nearest_t, Ray const &ray)
        {
            float ndotdir = dot(normal, ray.direction());
            if (fabs(ndotdir) < 1e-6f)
            {
                return false;
            }
            float d = -dot(normal, p0);
            float t = -(dot(ray.origin(), normal) + d) / ndotdir;
            if (t > nearest_t || t < 0)
            {
                return false;
            }
            vec3 P = ray.PointAtParameter(t);
            vec3 c;
            vec3 edge0 = p1 - p0;
            vec3 vp0 = P - p0;
            c = cross(edge0, vp0);
            if (dot(normal, c) < 0.0f)
            {
                return false;
            }
            vec3 edge1 = p2 - p1;
            vec3 vp1 = P - p1;
            c = cross(edge1, vp1);
            if (dot(normal, c) < 0.0f)
            {
                return false;
            }
            vec3 edge2 = p0 - p2;
            vec3 vp2 = P - p2;
            c = cross(edge2, vp2);
            if (dot(normal, c) < 0.0f)
            {
                return false;
            }
            return true;
        }
        [[nodiscard]] static bool Intersect(vec3 const &p0, vec3 const &p1, vec3 const &p2, float &nearest_t, Ray const &ray)
        {
            vec3 normal = cross(p1 - p0, p2 - p0);
            return Intersect(p0, p1, p2, normal, nearest_t, ray);
        }
        [[nodiscard]] static bool Intersect(vec3 const &p0, vec3 const &p1, vec3 const &p2, vec3 const &normal, Intersection &nearest, Ray const &ray)
        {
            float ndotdir = dot(normal, ray.direction());
            if (fabs(ndotdir) < 1e-6f)
            {
                return false;
            }
            float d = -dot(normal, p0);
            float t = -(dot(ray.origin(), normal) + d) / ndotdir;
            if (t > nearest.t || t < 0)
            {
                return false;
            }
            vec3 P = ray.PointAtParameter(t);
            vec3 c;
            vec3 edge0 = p1 - p0;
            vec3 vp0 = P - p0;
            c = cross(edge0, vp0);
            if (dot(normal, c) < 0.0f)
            {
                return false;
            }
            vec3 edge1 = p2 - p1;
            vec3 vp1 = P - p1;
            c = cross(edge1, vp1);
            if (dot(normal, c) < 0.0f)
            {
                return false;
            }
            vec3 edge2 = p0 - p2;
            vec3 vp2 = P - p2;
            c = cross(edge2, vp2);
            if (dot(normal, c) < 0.0f)
            {
                return false;
            }
            nearest.point = P;
            // reverse normal if the ray is on the opposite side of the triangle
            nearest.normal = normal * (ndotdir > 0 ? -1 : 1);
            nearest.t = t;
            return true;
        }
        [[nodiscard]] static bool Intersect(vec3 const &p0, vec3 const &p1, vec3 const &p2, Intersection &nearest, Ray const &ray)
        {
            vec3 normal = cross(p1 - p0, p2 - p0);
            return Intersect(p0, p1, p2, normal, nearest, ray);
        }

        [[nodiscard]] bool Intersect(float &nearest_t, Ray const &ray) const
        {
            return Intersect(a, b, c, nearest_t, ray);
        }
        [[nodiscard]] bool Intersect(float &nearest_t, Ray const &ray, vec3 const &normal) const
        {
            return Intersect(a, b, c, normal, nearest_t, ray);
        }
        [[nodiscard]] bool Intersect(Intersection &nearest, Ray const &ray) const
        {
            return Intersect(a, b, c, nearest, ray);
        }
        [[nodiscard]] bool Intersect(Intersection &nearest, Ray const &ray, vec3 const &normal) const
        {
            return Intersect(a, b, c, normal, nearest, ray);
        }
    };
}