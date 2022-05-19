#pragma once
#include "math.hpp"
namespace engine::components
{
  class Material final
  {
  public:
    math::vec3 color{0.6f};
  };
} // namespace engine::components