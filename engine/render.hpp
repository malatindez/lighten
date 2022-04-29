#pragma once
#include "pch.hpp"
namespace engine {
bool hit_sphere(math::vec3 const &center, float radius, math::ray const &r) {
  math::vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = 2.0f * dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - 4 * a * c;
  return discriminant > 0;
}

math::vec3 color(math::ray const &r) {
  if (hit_sphere(math::vec3{0, 0, -1}, 0.5f, r)) {
    return math::vec3{1, 0, 0};
  }
  math::vec3 unit_direction = r.direction().unit_vector();
  float t = 0.5f * (unit_direction.y() + 1);
  return (1.0f - t) * math::vec3{1} + t * math::vec3{0.5f, 0.7f, 1.0f};
}

void draw(std::vector<uint32_t> &bitmap, math::ivec2 window_size) {
  math::vec3 lower_left_corner{-2, -1, -1};
  math::vec3 horizontal{4, 0, 0};
  math::vec3 vertical{0, 2, 0};
  math::vec3 origin{0};
  for (int j = window_size.y() - 1; j >= 0; j--) {
    for (int i = 0; i < window_size.x(); i++) {
      float u = float(i) / float(window_size.x());
      float v = float(j) / float(window_size.y());
      math::ray r(origin, lower_left_corner + u * horizontal + v * vertical);
      math::vec3 col = color(r);
      auto ir = int(255.99 * col[0]) << 16;
      auto ig = int(255.99 * col[1]) << 8;
      auto ib = int(255.99 * col[2]);

      bitmap[size_t(j) * window_size.x() + i] = ir | ig | ib;
    }
  }
}
} // namespace engine