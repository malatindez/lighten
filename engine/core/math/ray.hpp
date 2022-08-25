#pragma once
#include "../math.hpp"
namespace engine::core::math
{
    class Ray
    {
    public:
        Ray() = default;
        Ray(vec3 const &origin, vec3 const &direction)
            : origin_(origin), direction_(direction) {
            OnDirecitonUpdate();
        }
        [[nodiscard]] constexpr vec3 const &origin() const noexcept { return origin_; }
        [[nodiscard]] constexpr vec3 &origin() noexcept { return origin_; }
        [[nodiscard]] constexpr vec3 const &direction() const noexcept { return direction_; }
        [[nodiscard]] constexpr vec3 const &inv_direction() const noexcept { return inv_direction_; }
        [[nodiscard]] constexpr ivec3 const &sign() const noexcept { return sign_; }
        void SetDirection(vec3 const &new_direction)
        {
            direction_ = new_direction;
            OnDirecitonUpdate();
        }
        [[nodiscard]] vec3 PointAtParameter(float t) const
        {
            return origin_ + direction_ * t;
        }

    private:
        void OnDirecitonUpdate()
        {
            inv_direction_ = 1 / direction_;
            sign_ = ivec3{ inv_direction_.x < 0, inv_direction_.y < 0, inv_direction_.z < 0 };
        }
        vec3 origin_;
        vec3 direction_;
        vec3 inv_direction_;
        ivec3 sign_;
    };
}; // namespace engine::core::math