#pragma once
#include "camera-controller.hpp"
namespace engine
{

    CameraController::CameraController(components::Camera &camera, components::Transform &transform,
                                       math::ivec2 const &window_size)
        : camera_(camera), transform_(transform), window_size_(window_size)
    {
        UpdateProjectionMatrix();
        UpdateMatrices();
    }
    void CameraController::UpdateProjectionMatrix()
    {
        SetProjectionMatrix(math::perspective(
            camera_.fovy_,
            static_cast<float>(window_size_.x) / static_cast<float>(window_size_.y),
            camera_.z_far_, camera_.z_near_));
    }

    void CameraController::SetProjectionMatrix(math::mat4 const &proj)
    {
        camera_.projection = proj;
        camera_.inv_projection = math::inverse(proj);
    }
    void CameraController::SetWorldOffset(math::vec3 const &offset)
    {
        transform_.position = offset; // overwrites
        transform_.UpdateMatrices();
        update_matrices_ = true;
    }

    void CameraController::AddWorldOffset(math::vec3 const &offset)
    {
        transform_.position += offset;
        transform_.UpdateMatrices();
        update_matrices_ = true;
    }

    void CameraController::AddRelativeOffset(math::vec3 const &offset)
    {
        UpdateBasis();
        transform_.position +=
            offset[0] * right_ + offset[1] * up_ + offset[2] * forward_;
        transform_.UpdateMatrices();
        update_matrices_ = true;
    }
    void CameraController::SetWorldAngles(float roll, float pitch, float yaw)
    {
        update_basis_ = true;
        update_matrices_ = true;
        transform_.rotation = math::quat(roll, math::vec3{0.f, 0.f, 1.f});
        transform_.rotation *= math::quat(pitch, math::vec3{1.f, 0.f, 0.f});
        transform_.rotation *= math::quat(yaw, math::vec3{0.f, 1.f, 0.f});
        transform_.rotation = normalize(transform_.rotation);
    }

    void CameraController::AddWorldAngles(float roll, float pitch, float yaw)
    {
        update_basis_ = true;
        update_matrices_ = true;
        transform_.rotation *= math::quat(roll, math::vec3{0.f, 0.f, 1.f});
        transform_.rotation *= math::quat(pitch, math::vec3{1.f, 0.f, 0.f});
        transform_.rotation *= math::quat(yaw, math::vec3{0.f, 1.f, 0.f});
        transform_.rotation = normalize(transform_.rotation);
    }

    void CameraController::AddRelativeAngles(float roll, float pitch, float yaw)
    {
        update_basis_ = true;
        update_matrices_ = true;
        transform_.rotation *= math::quat(roll, forward_);
        transform_.rotation *= math::quat(pitch, right_);
        transform_.rotation *= math::quat(yaw, up_);
        transform_.rotation = normalize(transform_.rotation);
    }

    void CameraController::UpdateBasis()
    {
        if (!update_basis_)
        {
            return;
        }
        update_basis_ = false;

        math::mat3 rotation = transform_.rotation.as_mat3();
        right_.x = camera_.inv_view[0].x = rotation[0].x;
        right_.y = camera_.inv_view[0].y = rotation[0].y;
        right_.z = camera_.inv_view[0].z = rotation[0].z;
        up_.x = camera_.inv_view[1].x = rotation[1].x;
        up_.y = camera_.inv_view[1].y = rotation[1].y;
        up_.z = camera_.inv_view[1].z = rotation[1].z;
        forward_.x = camera_.inv_view[2].x = rotation[2].x;
        forward_.y = camera_.inv_view[2].y = rotation[2].y;
        forward_.z = camera_.inv_view[2].z = rotation[2].z;
    }

    void CameraController::UpdateMatrices()
    {
        if (!update_matrices_)
        {
            return;
        }
        update_matrices_ = false;

        UpdateBasis();
        camera_.inv_view[3][0] = transform_.position[0];
        camera_.inv_view[3][1] = transform_.position[1];
        camera_.inv_view[3][2] = transform_.position[2];

        camera_.view = math::invert_orthonormal(camera_.inv_view);
        camera_.view_projection = camera_.view * camera_.projection;
        camera_.inv_view_projection = camera_.inv_projection * camera_.inv_view;
    }
} // namespace engine