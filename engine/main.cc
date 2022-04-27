#include <iostream>

#include "math/ray.hpp"
#include "math/vec3.hpp"
math::vec3 color(math::ray const& r) {
  math::vec3 unit_direction = r.direction().unit_vector();
  float t = 0.5 * (unit_direction.y() + 1);
  return (1.0 - t) * math::vec3{1, 1, 1} + t * math::vec3{0.5, 0.7, 1.0};
}

int main() {
  math::vec3 vec1{1, 2, 3};
  math::vec3 vec2{4, 6, 8};
  math::vec3 vec = vec1 + vec2;
  std::cout << vec.x() << " " << vec.y() << " " << vec.z() << " ";
}