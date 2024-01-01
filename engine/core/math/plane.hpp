#pragma once
#include "intersection.hpp"
#include "ray.hpp"
#include <stdexcept>
/**
 * @file plane.hpp
 *
 * @author malatindez (Vladyslav Ohlii)
 * @date 12/09/2023
 *
 * @brief Provides the definition of the Plane structure and its associated operations.
 *
 * The Plane structure represents a mathematical plane in 3D space. It provides functionality
 * to intersect the plane with a ray and to update the plane's definition based on two vectors.
 *
 * @see intersection.hpp
 * @see ray.hpp
 */

namespace engine::core::math
{
    /**
     * @struct Plane
     * @brief Represents a plane in 3D space defined by its normal and two vectors.
     *
     * This structure provides functionalities such as checking for intersections
     * with rays and updating the plane based on new vector inputs.
     */
    struct Plane
    {
        /**
         * @brief Checks for the intersection of the plane with a given ray.
         *
         * @param i Intersection data to update if an intersection occurs.
         * @param ray The ray to check for intersection with the plane.
         * @return True if the ray intersects the plane, false otherwise.
         */
        bool Intersect(Intersection &i, Ray const &ray) const
        {
            float denom = dot(normal, ray.direction());
            if (std::abs(denom) <= 1e-6f)
            {
                return false;
            }
            float t = dot(-ray.origin(), normal) / denom;
            if (t > i.t || t <= 0)
            {
                return false;
            }
            i.t = t;
            // reverse normal if the ray is on the opposite side of the plane
            i.normal = normal * (denom > 0 ? -1.0f : 1.0f);
            i.point = ray.PointAtParameter(t);
            return true;
        }
        /**
         * @brief Updates the plane's vectors and normal based on two new vectors.
         *
         * @param first The first vector to define the plane.
         * @param second The second vector to define the plane.
         * @throws std::invalid_argument if the provided vectors are collinear.
         */
        void update_plane(glm::vec3 const &first, glm::vec3 const &second)
        {
            glm::vec3 temp = -cross(first, second);
            if (length(temp) == 0)
            {
                throw std::invalid_argument("Input vectors can't be collinear!");
            }
            v = first;
            w = second;
            normal = normalize(temp);
        }
        glm::vec3 v;      ///< One of the vectors defining the plane.
        glm::vec3 w;      ///< The other vector defining the plane.
        glm::vec3 normal; ///< The normal to the plane.
    };
}