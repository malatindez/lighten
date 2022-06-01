#pragma once
#include "components/transform.hpp"
#include "math.hpp"
namespace engine::components
{
    class Camera
    {
    public:
        constexpr math::vec3 position() const noexcept { return math::vec3{inv_view[3][0], inv_view[3][1], inv_view[3][2]}; }
        math::mat4 view{1};
        math::mat4 projection{1};
        math::mat4 view_projection{1};
        math::mat4 inv_view{1};
        math::mat4 inv_projection{1};
        math::mat4 inv_view_projection{1};
        float fovy_ = math::radians(90.0f);
        float z_near_ = 0.02f;
        float z_far_ = 20.0f;
    };
}