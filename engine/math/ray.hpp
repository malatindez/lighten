#pragma once
#include "math/vec3.hpp"
namespace engine::math {
class ray {
 public:
  ray() = default;
  ray(vec3 const& origin, vec3 const& direction)
      : origin_(origin), direction_(direction) {}
  [[nodiscard]] vec3 const& origin() const noexcept { return origin_; }
  [[nodiscard]] vec3 const& direction() const noexcept { return direction_; }
  void SetOrigin(vec3 const& origin) noexcept { origin_ = origin; }
  void SetDirection(vec3 const& direction) noexcept { direction_ = direction; }
  [[nodiscard]] vec3 PointAtParameter(float t) const {
    return origin_ + direction_ * t;
  }

 private:
  vec3 origin_;
  vec3 direction_;
};
};  // namespace engine::math