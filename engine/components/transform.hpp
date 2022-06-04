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
      model = math::translate(model, position);
      model = model * rotation.as_mat4();
      model = math::scale(model, scale);
      inv_model = math::inverse(model);
    }
    math::vec3 position;
    float padding0;
    math::vec3 scale{1};
    float padding1;
    math::quat rotation;
    math::mat4 model;
    math::mat4 inv_model;
  };
} // namespace engine::components