#pragma once
#include "../math.hpp"
/**
 * @file ray.hpp
 * @brief Contains the definition of the Ray class.
 *
 * This file provides the Ray class, which represents a geometric ray in 3D space.
 * The Ray class is equipped with various methods for querying and modifying its origin
 * and direction, as well as computing a point at a parameter along the ray.

 */
namespace lighten::core::math
{
    /**
     * @class Ray
     * @brief Represents a geometric ray in 3D space.
     *
     * The Ray class defines a ray originating from a point, extending infinitely in a given direction.
     * It provides methods to query and set the origin and direction of the ray, as well as
     * retrieving information beneficial for certain algorithms like intersection tests,
     * such as the inverse direction and the sign.
     */
    class Ray
    {
    public:
        /// Default constructor.
        Ray() = default;
        /**
         * @brief Constructs a Ray with a given origin and direction.
         * @param origin Starting point of the ray.
         * @param direction Direction in which the ray extends.
         */
        Ray(glm::vec3 const &origin, glm::vec3 const &direction)
            : origin_(origin), direction_(direction)
        {
            OnDirectionUpdate();
        }
        /// Retrieves the ray's origin (const version).
        [[nodiscard]] constexpr glm::vec3 const &origin() const noexcept { return origin_; }
        /// Retrieves the ray's origin.
        [[nodiscard]] constexpr glm::vec3 &origin() noexcept { return origin_; }
        /// Retrieves the ray's direction (const version).
        [[nodiscard]] constexpr glm::vec3 const &direction() const noexcept { return direction_; }
        /// Retrieves the inverse of the ray's direction.
        [[nodiscard]] constexpr glm::vec3 const &inv_direction() const noexcept { return inv_direction_; }
        /// Retrieves the sign of the inverse direction for each axis.
        [[nodiscard]] constexpr glm::ivec3 const &sign() const noexcept { return sign_; }
        /**
         * @brief Sets a new direction for the ray and updates associated attributes.
         * @param new_direction The new direction for the ray.
         */
        void SetDirection(glm::vec3 const &new_direction)
        {
            direction_ = new_direction;
            OnDirectionUpdate();
        }
        /**
         * @brief Computes a point along the ray based on the given parameter.
         * @param t The parameter value.
         * @return A point on the ray at the given parameter.
         */
        [[nodiscard]] glm::vec3 PointAtParameter(float t) const
        {
            return origin_ + direction_ * t;
        }

    private:
        /// Updates attributes derived from the ray's direction.
        void OnDirectionUpdate()
        {
            inv_direction_ = 1.0f / direction_;
            sign_ = glm::ivec3{inv_direction_.x < 0, inv_direction_.y < 0, inv_direction_.z < 0};
        }
        glm::vec3 origin_;        ///< The ray's starting point.
        glm::vec3 direction_;     ///< The direction in which the ray extends.
        glm::vec3 inv_direction_; ///< The inverse of the ray's direction.
        glm::ivec3 sign_;         ///< Sign of the inverse direction for each axis.
    };
}; // namespace mal_math