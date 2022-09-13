#pragma once
#include "core/math.hpp"
namespace engine::components
{
#pragma warning(push)
#pragma warning(disable : 26495)
    struct TransformComponent
    {
        void reset()
        {
            position = core::math::vec3{ 0 };
            scale = core::math::vec3{ 1 };
            rotation = core::math::quat{ 1, 0, 0, 0 };
            UpdateMatrices();
        }
        void UpdateMatrices() noexcept
        {
            model = core::math::mat4{ 1 };
            model = core::math::translate(model, position);
            model = core::math::scale(model, scale);
            model = rotation.as_mat4() * model;
            inv_model = core::math::inverse(model);
        }
        core::math::vec3 position{ 0 };
        float padding0 = 0;
        core::math::vec3 scale{ 1 };
        float padding1 = 0;
        core::math::quat rotation;
        core::math::mat4 model;
        core::math::mat4 inv_model;
    };
#pragma warning(pop)

    struct CameraComponent
    {
        constexpr core::math::vec3 position() const noexcept
        {
            return core::math::vec3{ inv_view[3][0], inv_view[3][1], inv_view[3][2] };
        }
        core::math::mat4 view{ 1 };
        core::math::mat4 projection{ 1 };
        core::math::mat4 view_projection{ 1 };
        core::math::mat4 inv_view{ 1 };
        core::math::mat4 inv_projection{ 1 };
        core::math::mat4 inv_view_projection{ 1 };
        float fovy_ = core::math::radians(45.0f);
        float z_near_ = 200.0f;
        float z_far_ = 0.002f;
    };
} // namespace engine::components