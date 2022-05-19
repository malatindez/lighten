#pragma once
#include "math.hpp"
namespace engine::components
{
  class Transform final
  {
  public:
    math::vec3 position{0};
    math::vec3 scale{1};
    math::quat rotation{1,0,0,0};
    void reset()
    {
      position = math::vec3{0};
      scale = math::vec3{1};
      rotation = math::quat{1,0,0,0};
    }
    [[nodiscard]] math::mat4 model() const noexcept 
    { 
      math::mat4 mat(1);
      mat = math::scale(mat, scale);
      mat = math::rotate(mat, rotation);
      mat = math::translate(mat, position);
      return mat;
    };
  };
} // namespace engine::components