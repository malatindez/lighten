#pragma once
#include "components/transform.hpp"
#include "math.hpp"
namespace engine
{
    class Camera
    {
    public:
        math::mat4 view_;
        math::mat4 transform_;
        math::mat4 view_projection_;
        math::mat4 inv_view_;
        math::mat4 inv_transform_;
        math::mat4 inv_view_projection_;

    };
}