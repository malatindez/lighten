#pragma once
#include "components/transform.hpp"
#include "math.hpp"
namespace engine::components
{
    class Camera
    {
    public:
        math::mat4 view{1};
        math::mat4 projection{1};
        math::mat4 view_projection{1};
        math::mat4 inv_view{1};
        math::mat4 inv_projection{1};
        math::mat4 inv_view_projection{1};
    };
}