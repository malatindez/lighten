#pragma once
#include "../math.hpp"
namespace engine::core::math
{
    struct Triangle
    {
        vec3 a;
        vec3 b;
        vec3 c;
        [[nodiscard]] static bool Intersect(vec3 const &p0, vec3 const &p1, vec3 const &p2, float &nearest_t, Ray const &ray)
        {
            const float EPSILON = 0.0000001f;
            vec3 edge1, edge2, h, s, q;
            float a, f, u, v;
            edge1 = p1 - p0;
            edge2 = p2 - p0;
            h = cross(ray.direction(), edge2);
            a = dot(edge1, h);
            if (a > -EPSILON && a < EPSILON)
                return false;    // This ray is parallel to this triangle.
            f = 1.0f / a;
            s = ray.origin() - p0;
            u = f * dot(s, h);
            if (u < 0.0f || u > 1.0f)
                return false;
            q = cross(s, edge1);
            v = f * dot(ray.direction(), q);
            if (v < 0.0f || u + v > 1.0f)
                return false;
            // At this stage we can compute t to find out where the intersection point is on the line.
            float t = f * dot(edge2, q);
            if (t > EPSILON && t < nearest_t) // ray intersection
            {
                nearest_t = t;
                return true;
            }
            else // This means that there is a line intersection but not a ray intersection.
                return false;
        }
        [[nodiscard]] static bool Intersect(vec3 const &p0, vec3 const &p1, vec3 const &p2, Intersection &nearest, Ray const &ray)
        {
            const float EPSILON = 0.0000001f;
            vec3 edge1, edge2, h, s, q;
            float a, f, u, v;
            edge1 = p1 - p0;
            edge2 = p2 - p0;
            h = cross(ray.direction(), edge2);
            a = dot(edge1, h);
            if (a > -EPSILON && a < EPSILON)
                return false;    // This ray is parallel to this triangle.
            f = 1.0f / a;
            s = ray.origin() - p0;
            u = f * dot(s, h);
            if (u < 0.0f || u > 1.0f)
                return false;
            q = cross(s, edge1);
            v = f * dot(ray.direction(), q);
            if (v < 0.0f || u + v > 1.0f)
                return false;
            // At this stage we can compute t to find out where the intersection point is on the line.
            float t = f * dot(edge2, q);
            if (t > EPSILON && t < nearest.t) // ray intersection
            {
                nearest.t = t;
                nearest.point = ray.PointAtParameter(t);
                return true;
            }
            else // This means that there is a line intersection but not a ray intersection.
                return false;
        }
        [[nodiscard]] bool Intersect(float &nearest_t, Ray const &ray) const
        {
            return Intersect(a, b, c, nearest_t, ray);
        }
        [[nodiscard]] bool Intersect(Intersection &nearest, Ray const &ray) const
        {
            return Intersect(a, b, c, nearest, ray);
        }
    };
}