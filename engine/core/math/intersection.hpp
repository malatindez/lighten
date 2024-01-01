/**
 * @file intersection.hpp
 * @author malatindez (Vladyslav Ohlii)
 * @date 12/09/2023
 * 
 * @brief Provides functionalities to represent and handle intersections in 3D space.
 * 
 * This header establishes the foundational structures and utilities necessary for 
 * representing the intersections of rays with various objects in three-dimensional space.
 * Central to this is the `Intersection` structure which encapsulates pertinent details 
 * about an intersection point, such as the intersection parameter `t`, the actual 
 * intersection point, and the normal at the intersection.
 * 
 * The Intersection struct is designed to be lightweight, efficient, and intuitive. It 
 * provides several utility functions that allow users to reset intersections, check for 
 * the existence of intersections based on the `t` parameter, and even create intersections 
 * that represent infinite or non-existent intersections.
 * 
 * Example:
 * ```cpp
 * Intersection intersect;
 * intersect.reset();   // Sets the intersection parameter t to infinity
 * 
 * ...
 * 
 * * check for intersection *
 * 
 * if (intersect.exists()) { 
 *     // Handle the intersection 
 * }
 * ```
 * 
 * For advanced geometric operations and further intersection types, users are encouraged 
 * to explore the broader `mal_math` namespace and associated headers.
 */

#pragma once
#include "../math.hpp"

#pragma warning(push)
#pragma warning(disable : 26495)

namespace engine::core::math
{
    /**
     * @brief Represents the data of a ray's intersection with an object.
     * 
     * The Intersection structure encapsulates details of an intersection point,
     * such as the intersection parameter `t`, the intersection point itself,
     * and the normal at the intersection.
     */
    struct Intersection
    {
        float t;       ///< The intersection parameter.
        vec3 point;    ///< The 3D point of intersection.
        vec3 normal;   ///< The 3D normal at the point of intersection.

        constexpr Intersection() = default;
        constexpr Intersection(Intersection const &) = default;
        constexpr Intersection &operator=(Intersection const &) = default;
        constexpr Intersection(Intersection &&) = default;
        constexpr Intersection &operator=(Intersection &&) = default;

        /**
         * @brief Resets the intersection data.
         * 
         * The intersection parameter `t` is set to infinity.
         */
        constexpr void reset() { t = std::numeric_limits<float>::infinity(); }

        /**
         * @brief Determines if the intersection exists based on the value of `t`.
         * 
         * @return true If `t` is finite, indicating an intersection exists.
         * @return false Otherwise.
         */
        inline bool exists() const { return std::isfinite(t); }

        /**
         * @brief Creates and returns an intersection with `t` set to infinity.
         * 
         * @return Intersection An intersection object representing no intersection.
         */
        static constexpr Intersection infinite()
        {
            Intersection inf;
            inf.reset();
            return inf;
        }
    };
} // namespace mal_math

#pragma warning(pop)
