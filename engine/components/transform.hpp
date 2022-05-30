#pragma once
#include "math.hpp"
namespace engine::components
{
  class Transform final
  {
  public:
    void reset()
    {
      position = math::vec3{0};
      scale = math::vec3{1};
      rotation = math::quat{1, 0, 0, 0};
      UpdateMatrices();
    }
    void UpdateMatrices() noexcept
    {
      model = math::mat4{1};
      model = math::scale(model, scale);
      model = model * rotation.as_mat4();
      model = math::translate(model, position);
      inv_model = math::inverse(model);
    }
    math::vec3 position{0};
    math::vec3 scale{1};
    math::quat rotation{1, 0, 0, 0};
    math::mat4 model{1};
    math::mat4 inv_model{1};
  };
} // namespace engine::components