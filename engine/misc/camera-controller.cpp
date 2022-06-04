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
            float(window_size_.x) / float(window_size_.y),
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
        transform_.position += offset[0] * right() + offset[1] * up() + offset[2] * forward();
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
        transform_.rotation *= math::quat(roll, forward().as_vec());
        transform_.rotation *= math::quat(pitch, right().as_vec());
        transform_.rotation *= math::quat(yaw, up().as_vec());
        transform_.rotation = normalize(transform_.rotation);
    }

    void CameraController::UpdateBasis()
    {
        if (!update_basis_)
        {
            return;
        }
        update_basis_ = false;

        camera_.inv_view.as_rmat<3, 3>() = transform_.rotation.as_mat3();
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