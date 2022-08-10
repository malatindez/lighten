#pragma once
#include "../math.hpp"
namespace engine::core::math
{
    class Ray
    {
    public:
        Ray() = default;
        Ray(vec3 const &origin, vec3 const &direction)
            : origin_(origin), direction_(direction) {}
        [[nodiscard]] vec3 const &origin() const noexcept { return origin_; }
        [[nodiscard]] vec3 const &direction() const noexcept { return direction_; }
        [[nodiscard]] vec3 &origin() noexcept { return origin_; }
        [[nodiscard]] vec3 &direction() noexcept { return direction_; }
        [[nodiscard]] vec3 PointAtParameter(float t) const
        {
            return origin_ + direction_ * t;
        }

    private:
        vec3 origin_;
        vec3 direction_;
    };
}; // namespace engine::core::math