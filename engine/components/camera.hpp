#pragma once
#include "components/transform.hpp"
namespace engine::components
{
    class Camera
    {
    public:
        constexpr core::math::vec3 position() const noexcept
        {
            return core::math::vec3 { inv_view[3][0], inv_view[3][1], inv_view[3][2] };
        }
        core::math::mat4 view { 1 };
        core::math::mat4 projection { 1 };
        core::math::mat4 view_projection { 1 };
        core::math::mat4 inv_view { 1 };
        core::math::mat4 inv_projection { 1 };
        core::math::mat4 inv_view_projection { 1 };
        float fovy_ = core::math::radians(45.0f);
        float z_near_ = 0.02f;
        float z_far_ = 20.0f;
    };
} // namespace engine::components