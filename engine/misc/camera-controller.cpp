#include "camera-controller.hpp"
namespace engine
{
    using namespace core;
    using namespace math;

    CameraController::CameraController(components::Camera *camera, components::Transform *transform,
                                       ivec2 const &window_size)
        : camera_(camera), transform_(transform), window_size_(window_size)
    {
        if (camera_ != nullptr && transform_ != nullptr)
        {
            Init();
        }
    }
    void CameraController::Init()
    {
        UpdateProjectionMatrix();
        UpdateMatrices();
    }
    void CameraController::UpdateProjectionMatrix()
    {
        SetProjectionMatrix(perspective(
            camera_->fovy_,
            float(window_size_.x) / float(window_size_.y),
            camera_->z_near_, camera_->z_far_));
        update_matrices_ = true;
    }

    void CameraController::SetProjectionMatrix(mat4 const &proj)
    {
        camera_->projection = proj;
        camera_->inv_projection = inverse(proj);
    }
    void CameraController::SetWorldOffset(vec3 const &offset)
    {
        transform_->position = offset; // overwrites
        transform_->UpdateMatrices();
        update_matrices_ = true;
    }

    void CameraController::AddWorldOffset(vec3 const &offset)
    {
        transform_->position += offset;
        transform_->UpdateMatrices();
        update_matrices_ = true;
    }

    void CameraController::AddRelativeOffset(vec3 const &offset)
    {
        UpdateBasis();
        transform_->position += offset[0] * right() + offset[1] * up() + offset[2] * forward();
        transform_->UpdateMatrices();
        update_matrices_ = true;
    }

    void CameraController::SetWorldAngles(float roll, float pitch, float yaw)
    {
        update_basis_ = true;
        update_matrices_ = true;
        if (roll_enabled_)
        {
            transform_->rotation = quat(roll, vec3 { 0.f, 0.f, 1.f });
        }
        transform_->rotation *= quat(pitch, vec3 { 1.f, 0.f, 0.f });
        transform_->rotation *= quat(yaw, vec3 { 0.f, 1.f, 0.f });
        transform_->rotation = normalize(transform_->rotation);
    }

    void CameraController::AddWorldAngles(float roll, float pitch, float yaw)
    {
        update_basis_ = true;
        update_matrices_ = true;
        if (roll_enabled_)
        {
            transform_->rotation *= quat(roll, vec3 { 0.f, 0.f, 1.f });
        }
        transform_->rotation *= quat(pitch, vec3 { 1.f, 0.f, 0.f });
        transform_->rotation *= quat(yaw, vec3 { 0.f, 1.f, 0.f });
        transform_->rotation = normalize(transform_->rotation);
    }

    void CameraController::AddRelativeAngles(float roll, float pitch, float yaw)
    {
        update_basis_ = true;
        update_matrices_ = true;
        if (roll_enabled_)
        {
            transform_->rotation *= quat(roll, forward().as_vec());
            transform_->rotation *= quat(pitch, right().as_vec());
            transform_->rotation *= quat(yaw, up().as_vec());
        }
        else
        {
            transform_->rotation *= quat(pitch, right().as_vec());
            transform_->rotation *= quat(yaw, vec3 { 0, 1, 0 });
        }
        transform_->rotation = normalize(transform_->rotation);
    }

    void CameraController::UpdateBasis()
    {
        if (!update_basis_)
        {
            return;
        }
        update_basis_ = false;

        camera_->inv_view.as_rmat<3, 3>() = transform_->rotation.as_mat3();
    }

    void CameraController::UpdateMatrices()
    {
        if (!update_matrices_)
        {
            return;
        }
        update_matrices_ = false;

        UpdateBasis();
        camera_->inv_view[3].as_rvec<3>() = transform_->position;

        camera_->view = invert_orthonormal(camera_->inv_view);
        camera_->view_projection = camera_->view * camera_->projection;
        camera_->inv_view_projection = camera_->inv_projection * camera_->inv_view;
    }
    namespace
    {
        const vec3 kUp { 0, 1, 0 };
        const vec3 kDown { 0, -1, 0 };
        const vec3 kLeft { -1, 0, 0 };
        const vec3 kRight { 1, 0, 0 };
        const vec3 kForward { 0, 0, 1 };
        const vec3 kBackwards { 0, 0, -1 };
    }
    void CameraController::OnTick(float delta_time, core::math::ivec2 const &pixel_mouse_delta)
    {

        vec3 offset { 0 };
        // process movement
        if (flags_ & MoveForward)
        {
            offset += kForward;
        }
        if (flags_ & MoveBackwards)
        {
            offset += kBackwards;
        }
        if (flags_ & MoveLeft)
        {
            offset += kLeft;
        }
        if (flags_ & MoveRight)
        {
            offset += kRight;
        }
        if (flags_ & MoveDown)
        {
            offset += kDown;
        }
        if (flags_ & MoveUp)
        {
            offset += kUp;
        }

        float roll = 0;
        float yaw = 0;
        float pitch = 0;
        // process camera rotation
        if (roll_enabled())
        {
            if (flags_ & RotateLeft)
            {
                roll -= roll_speed_;
            }
            if (flags_ & RotateRight)
            {
                roll += roll_speed_;
            }
        }
        else
        {
            if (flags_ & RotateLeft)
            {
                offset += kDown;
            }
            if (flags_ & RotateRight)
            {
                offset += kUp;
            }
        }
        vec2 t { pixel_mouse_delta };
        t = -t / window_size_;
        t *= sensivity_ * camera().fovy_;
        yaw = t.x;
        pitch = t.y;
        if (!(roll == 0 && pitch == 0 && yaw == 0))
        {
            AddRelativeAngles(delta_time * roll, delta_time * pitch, delta_time * yaw);
        }
        if (squared_length(offset) != 0)
        {
            AddRelativeOffset(move_speed_ * offset * delta_time * ((flags_ & Accelerate) ? accelerated_speed_ : move_speed_));
        }
        UpdateMatrices();
        flags_ = None;
    }
} // namespace engine