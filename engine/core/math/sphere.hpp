#pragma once
#include "ray.hpp"
/**
 * @file sphere.hpp
 *
 * @author malatindez (Vladyslav Ohlii)
 * @date 12/09/2023
 *
 * @brief Defines the Sphere structure and its associated functionality.
 */
namespace engine::core::math
{
    /**
     * @struct Sphere
     * @brief A geometric representation of a sphere in 3D space.
     */
    struct Sphere
    {
        /**
         * @brief Determines the intersection point of a Ray with the Sphere.
         *
         * @param r The ray used for the intersection.
         * @param center The center of the sphere.
         * @param radius The radius of the sphere.
         * @return float Returns the t-value at which the Ray intersects the Sphere. Returns -1.0f if there's no intersection.
         */
        static constexpr float Hit(Ray const &r, glm::vec3 const &center, float radius) noexcept
        {
            const glm::vec3 oc = r.origin() - center;
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

        /**
         * @brief Determines if a Ray intersects with the Sphere.
         *
         * @param i Contains intersection information that will be updated if an intersection occurs.
         * @param ray The ray used for the intersection.
         * @return bool Returns true if there is an intersection, otherwise false.
         */
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
            i.normal = i.normal * (length(ray.origin()) < 1 ? -1.0f : 1.0f);
            return true;
        }

        glm::vec3 position{0}; ///< Position of the sphere's center in 3D space.
        float radius{1};       ///< Radius of the sphere.
    };
}