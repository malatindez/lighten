#pragma once
#include "../math.hpp"
/**
 * @file triangle.hpp
 *
 * @author malatindez (Vladyslav Ohlii)
 * @date 12/09/2023
 *
 * @brief Defines the Triangle structure and methods for ray-triangle intersection.
 */
namespace lighten::core::math
{
    /**
     * @struct Triangle
     * @brief Represents a triangle defined by three 3D points.
     */
    struct Triangle
    {
        glm::vec3 a; ///< First vertex of the triangle.
        glm::vec3 b; ///< Second vertex of the triangle.
        glm::vec3 c; ///< Third vertex of the triangle
        /**
         * @brief Determines if a ray intersects with the triangle using its vertices.
         *
         * @param p0 First vertex of the triangle.
         * @param p1 Second vertex of the triangle.
         * @param p2 Third vertex of the triangle.
         * @param nearest_t Distance from the ray's origin to the intersection point.
         * @param ray The ray for intersection testing.
         * @return bool Returns true if there is an intersection, otherwise false.
         */
        [[nodiscard]] static bool Intersect(glm::vec3 const &p0, glm::vec3 const &p1, glm::vec3 const &p2, float &nearest_t, Ray const &ray)
        {
            const float EPSILON = 0.0000001f;
            glm::vec3 edge1, edge2, h, s, q;
            float a, f, u, v;
            edge1 = p1 - p0;
            edge2 = p2 - p0;
            h = cross(ray.direction(), edge2);
            a = dot(edge1, h);
            if (a > -EPSILON && a < EPSILON)
                return false; // This ray is parallel to this triangle.
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
        /**
         * @brief Determines if a ray intersects with the triangle and updates intersection details.
         *
         * @param p0 First vertex of the triangle.
         * @param p1 Second vertex of the triangle.
         * @param p2 Third vertex of the triangle.
         * @param nearest Contains intersection information that will be updated if an intersection occurs.
         * @param ray The ray for intersection testing.
         * @return bool Returns true if there is an intersection, otherwise false.
         */
        [[nodiscard]] static bool Intersect(glm::vec3 const &p0, glm::vec3 const &p1, glm::vec3 const &p2, Intersection &nearest, Ray const &ray)
        {
            if (Intersect(p0, p1, p2, nearest.t, ray))
            {
                nearest.point = ray.PointAtParameter(nearest.t);
                return true;
            }
            return false;
        }
        /**
         * @brief Determines if a ray intersects with this triangle.
         *
         * @param nearest_t Distance from the ray's origin to the intersection point.
         * @param ray The ray for intersection testing.
         * @return bool Returns true if there is an intersection, otherwise false.
         */
        [[nodiscard]] bool Intersect(float &nearest_t, Ray const &ray) const
        {
            return Intersect(a, b, c, nearest_t, ray);
        }
        /**
         * @brief Determines if a ray intersects with this triangle and updates intersection details.
         *
         * @param nearest Contains intersection information that will be updated if an intersection occurs.
         * @param ray The ray for intersection testing.
         * @return bool Returns true if there is an intersection, otherwise false.
         */
        [[nodiscard]] bool Intersect(Intersection &nearest, Ray const &ray) const
        {
            return Intersect(a, b, c, nearest, ray);
        }
    };
}