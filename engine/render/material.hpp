#pragma once
#include "math.hpp"
namespace engine::render
{
  class Material final
  {
  public:
    math::vec3 albedo{1.0f};
    math::vec3 emission{0.0f};
    float specular = 1;
    float glossiness = 8;
    bool casts_shadow = true;
  };
} // namespace engine::components