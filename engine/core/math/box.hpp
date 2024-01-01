#pragma once
/**
 * @file box.hpp
 * @brief Defines a templated bounding box class for 3D primitives.
 */

#include "../math.hpp"

namespace engine::core::math
{
    /**
     * @brief A templated bounding box structure in 3D.
     *
     * TBox is a structure that encapsulates a box defined by a minimum and maximum point in 3D space.
     *
     * @tparam T The primitive type used to define the box coordinates (e.g., float, double).
     */
    template <typename T>
    struct TBox
    {
        using vector3 = glm::vec<3, T>; ///< Type alias for 3D vectors.

        vector3 min; ///< The minimum point of the box in 3D space.
        vector3 max; ///< The maximum point of the box in 3D space.

        /// @brief Infinity value for the primitive type used to define the box coordinates.
        static constexpr T Inf = std::numeric_limits<T>::infinity();

        /**
         * @brief Returns an empty box.
         *
         * @return TBox An empty box with its minimum point set to positive infinity and its maximum point set to negative infinity.
         */
        static constexpr TBox empty() { return {vector3{Inf, Inf, Inf}, vector3{-Inf, -Inf, -Inf}}; }

        /**
         * @brief Returns a unit box.
         *
         * @return TBox A box that spans from [-1, -1, -1] to [1, 1, 1].
         */
        static constexpr TBox unit() { return {vector3{-1.f, -1.f, -1.f}, vector3{1.f, 1.f, 1.f}}; }

        constexpr vector3 size() const { return max - min; }           ///< Returns the dimensions of the box.
        constexpr vector3 center() const { return (min + max) / 2.f; } ///< Returns the center point of the box.
        constexpr T radius() const { return length(size()) / 2.f; }    ///< Returns the radius of a sphere that tightly encloses the box.

        /**
         * @brief Resets the box to its default state.
         */
        void reset()
        {
            constexpr T maxT = std::numeric_limits<T>::max();
            min = vector3{maxT, maxT, maxT};
            max = -min;
        }

        /**
         * @brief Expands the box to include another box.
         *
         * @tparam U Type of the box to be used for expansion.
         * @param other Another bounding box.
         */
        template <typename U>
        void expand(const TBox<U> &other)
        {
            expand(other.min);
            expand(other.max);
        }
        /**
         * @brief Expands the box to include a point.
         *
         * @tparam U Type of the point to be used for expansion.
         * @param point A 3D point.
         */
        template <typename U>
        void expand(const U &point)
            requires(U::size == 3)
        {
            rmin(min, point);
            rmax(max, point);
        }

        /**
         * @brief Checks if the box contains a given point.
         *
         * @param P A 3D point.
         * @return true If the box contains the point.
         * @return false Otherwise.
         */
        bool contains(const vector3 &P) const
        {
            return min[0] <= P[0] && P[0] <= max[0] &&
                   min[1] <= P[1] && P[1] <= max[1] &&
                   min[2] <= P[2] && P[2] <= max[2];
        }

        /**
         * @brief Checks if a ray intersects with the box and updates the nearest intersection distance.
         *
         * @param ray The ray to be checked for intersection.
         * @param[out] nearest_t The nearest intersection distance is stored here if an intersection occurs.
         * @return true If the ray intersects with the box.
         * @return false Otherwise.
         */
        inline bool Intersect(Ray const &ray, T &nearest_t) const
        {
            float tmin, tmax, tymin, tymax, tzmin, tzmax;

            auto const &bounds = reinterpret_cast<std::array<vector3, 2> const &>(*this);

            tmin = (bounds[ray.sign()[0]].x - ray.origin().x) * ray.inv_direction().x;
            tmax = (bounds[1ll - ray.sign()[0]].x - ray.origin().x) * ray.inv_direction().x;
            tymin = (bounds[ray.sign()[1]].y - ray.origin().y) * ray.inv_direction().y;
            tymax = (bounds[1ll - ray.sign()[1]].y - ray.origin().y) * ray.inv_direction().y;

            if ((tmin > tymax) || (tymin > tmax))
                return false;

            if (tymin > tmin)
                tmin = tymin;
            if (tymax < tmax)
                tmax = tymax;

            tzmin = (bounds[ray.sign()[2]].z - ray.origin().z) * ray.inv_direction().z;
            tzmax = (bounds[1ll - ray.sign()[2]].z - ray.origin().z) * ray.inv_direction().z;

            if ((tmin > tzmax) || (tzmin > tmax))
                return false;

            if (tzmin > tmin)
                tmin = tzmin;
            if (tzmax < tmax)
                tmax = tzmax;

            float t = tmin;

            if (t < 0)
            {
                t = tmax;
                if (t < 0 || t > nearest_t)
                    return false;
            }
            nearest_t = t;
            return true;
        }

        /**
         * @brief Checks if a ray intersects with the box and updates the nearest intersection point.
         *
         * @param ray The ray to be checked for intersection.
         * @param[out] nearest The nearest intersection point is stored here if an intersection occurs.
         * @return true If the ray intersects with the box.
         * @return false Otherwise.
         */
        inline bool Intersect(Ray const &ray, Intersection &nearest) const
        {
            if (Intersect(ray, nearest.t))
            {
                nearest.point = ray.PointAtParameter(nearest.t);
                return true;
            }
            return false;
        }
    };

    using Box = TBox<float>;   ///< Box defined with floats.
    using Boxf = TBox<float>;  ///< Same as Box.
    using Boxd = TBox<double>; ///< Box defined with doubles.
}