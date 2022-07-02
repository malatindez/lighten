#pragma once
#include "core/math.hpp"
#pragma warning(push)
#pragma warning(disable : 26495)
namespace engine::components
{
  class Transform final
  {
  public:
    void reset()
    {
      position = core::math::vec3{0};
      scale = core::math::vec3{1};
      rotation = core::math::quat{1, 0, 0, 0};
      UpdateMatrices();
    }
    void UpdateMatrices() noexcept
    {
      model = core::math::mat4{1};
      model = core::math::translate(model, position);
      model = rotation.as_mat4() * model;
      model = core::math::scale(model, scale);
      inv_model = core::math::inverse(model);
    }
    core::math::vec3 position{0};
    float padding0 = 0;
    core::math::vec3 scale{1};
    float padding1 = 0;
    core::math::quat rotation;
    core::math::mat4 model;
    core::math::mat4 inv_model;
  };
#pragma warning(pop)
} // namespace engine::components