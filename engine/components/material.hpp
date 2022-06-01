#pragma once
#include "math.hpp"
namespace engine::components
{
  class Material final
  {
  public:
    math::vec3 albedo{0.6f};
    math::vec3 emission{0.0f};
    float specular;
    float glossiness;
    bool processes_light = true;
  };
} // namespace engine::components