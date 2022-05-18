#pragma once
#include "components/transform.hpp"
#include "math.hpp"
namespace engine::components
{
    class Camera
    {
    public:
        math::mat4 view;
        math::mat4 projection;
        math::mat4 view_projection;
        math::mat4 inv_view;
        math::mat4 inv_projection;
        math::mat4 inv_view_projection;
        math::quat rotation;
    };
}