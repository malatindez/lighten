#pragma once
#include "pch.hpp"
namespace engine::math {
class Sphere {
public:
  Sphere(math::vec3 center, float radius) : center_(center), radius_(radius) {}
  bool Hit(math::Ray const &r) const noexcept {
    math::vec3 oc = r.origin() - center_;
    // find sin of an angle between OC and d, where OC is the vector between ray
    // origin and the center of the sphere, and d is the direction of the ray
    float sinAngle =
        (dot(oc, r.direction()) /
         std::sqrt(oc.squared_length() * r.direction().squared_length()));
    sinAngle = std::sqrt(1 - sinAngle * sinAngle);
    return sinAngle * oc.length() <= radius_;
  }
  [[nodiscard]] inline float radius() const noexcept { return radius_; }
  [[nodiscard]] inline math::vec3 center() const noexcept { return center_; }
  void SetRadius(float radius) noexcept { radius_ = radius; }
  void SetCenter(math::vec3 center) noexcept { center_ = center; }

private:
  math::vec3 center_;
  float radius_;
};
} // namespace engine::math