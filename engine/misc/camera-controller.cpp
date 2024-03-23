#include "camera-controller.hpp"
#include "mal-toolkit/mal-toolkit.hpp"
namespace lighten
{
    using namespace core;
    using namespace math;

    CameraController::CameraController(entt::registry *registry, entt::entity entity,
                                       glm::ivec2 const &window_size)
        : registry_(registry), camera_(entity), window_size_(window_size),
        camera_update_observer{ *registry, entt::collector.update<components::Transform>().update<components::Camera>() }
    {
        Init();
    }
    void CameraController::Init()
    {
        local_camera_ = registry_->get<components::Camera>(camera_);
        local_transform_ = registry_->get<components::Transform>(camera_);
        UpdateProjectionMatrix();
        UpdateMatrices();
    }
    void CameraController::UpdateProjectionMatrix()
    {
        auto &cam = local_camera_;
        SetProjectionMatrix(glm::perspectiveLH_ZO(
            cam.fovy_,
            float(window_size_.x) / float(window_size_.y),
            cam.z_near_, cam.z_far_));
        update_matrices_ = true;
    }

    void CameraController::SetProjectionMatrix(glm::mat4 const &proj)
    {
        auto &cam = local_camera_;
        cam.projection = proj;
        cam.inv_projection = inverse(proj);
    }
    void CameraController::SetWorldOffset(glm::vec3 const &offset)
    {
        auto &transform_ = local_transform_;
        transform_.position = offset; // overwrites
        update_matrices_ = true;
    }

    void CameraController::AddWorldOffset(glm::vec3 const &offset)
    {
        auto &transform_ = local_transform_;
        transform_.position += offset;
        update_matrices_ = true;
    }

    void CameraController::AddRelativeOffset(glm::vec3 const &offset)
    {
        UpdateBasis();
        auto &transform_ = local_transform_;
        transform_.position += offset[0] * right() + offset[1] * up() + offset[2] * forward();
        update_matrices_ = true;
    }

    void CameraController::SetWorldAngles(float roll, float pitch, float yaw)
    {
        update_basis_ = true;
        update_matrices_ = true;
        auto &transform_ = local_transform_;
        if (roll_enabled_)
        {
            transform_.rotation = glm::quat(glm::vec3{0.f, 0.f, roll});
        }
        transform_.rotation *= glm::quat(glm::vec3{pitch, 0.f, 0.f});
        transform_.rotation *= glm::quat(glm::vec3{0.f, yaw, 0.f});
        transform_.rotation = normalize(transform_.rotation);
    }

    void CameraController::AddWorldAngles(float roll, float pitch, float yaw)
    {
        update_basis_ = true;
        update_matrices_ = true;
        auto &transform_ = local_transform_;
        if (roll_enabled_)
        {
            transform_.rotation *= glm::quat(glm::vec3{0.f, 0.f, roll});
        }
        transform_.rotation *= glm::quat(glm::vec3{pitch, 0.f, 0.f});
        transform_.rotation *= glm::quat(glm::vec3{0.f, yaw, 0.f});
        transform_.rotation = normalize(transform_.rotation);
    }

    void CameraController::AddRelativeAngles(float roll, float pitch, float yaw)
    {
        update_basis_ = true;
        update_matrices_ = true;
        auto &transform_ = local_transform_;
        if (roll_enabled_)
        {
            transform_.rotation *= glm::quat(roll * forward());
            transform_.rotation *= glm::quat(pitch * right());
            transform_.rotation *= glm::quat(yaw * up());
        }
        else
        {
            transform_.rotation *= glm::quat(pitch * right());
            transform_.rotation *= glm::quat(glm::vec3{0.0f, yaw, 0.0f});
        }
        transform_.rotation = normalize(transform_.rotation);
    }

    void CameraController::UpdateBasis()
    {
        if (!update_basis_)
        {
            return;
        }
        update_basis_ = false;
        auto &cam = local_camera_;
        auto &transform_ = local_transform_;
        auto t = glm::transpose(glm::mat3_cast(transform_.rotation));
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                cam.inv_view[i][j] = t[i][j];
            }
        }
    }

    void CameraController::UpdateMatrices()
    {
        if (!update_matrices_)
        {
            return;
        }
        update_matrices_ = false;

        UpdateBasis();
        auto &cam = local_camera_;
        auto &transform_ = local_transform_;
        cam.inv_view[3][0] = transform_.position.x;
        cam.inv_view[3][1] = transform_.position.y;
        cam.inv_view[3][2] = transform_.position.z;

        cam.view = invert_orthonormal(cam.inv_view);
        cam.view_projection = cam.projection * cam.view;
        cam.inv_view_projection = cam.inv_view * cam.inv_projection;
    }
    namespace
    {
        const glm::vec3 kUp{0, 1, 0};
        const glm::vec3 kDown{0, -1, 0};
        const glm::vec3 kLeft{-1, 0, 0};
        const glm::vec3 kRight{1, 0, 0};
        const glm::vec3 kForward{0, 0, 1};
        const glm::vec3 kBackwards{0, 0, -1};
    }
    void CameraController::OnTick(float delta_time, glm::ivec2 const &pixel_mouse_delta)
    {
        if (!camera_update_observer.empty())
        {
            for(auto entity : camera_update_observer)
			{
                if (entity != camera_)
                {
                    continue;
                }
				auto &cam = registry_->get<components::Camera>(entity);
                auto &transform = registry_->get<components::Transform>(entity);
				local_camera_ = cam;
                local_transform_ = transform;
				update_matrices_ = true;
                update_basis_ = true;
			}
        }
        glm::vec3 offset{0};
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
        glm::vec2 t{pixel_mouse_delta};
        t = -t / glm::vec2(window_size_);
        t *= sensitivity_ * local_camera_.fovy_;
        yaw = t.x;
        pitch = t.y;
        if (!(roll == 0 && pitch == 0 && yaw == 0))
        {
            AddRelativeAngles(delta_time * roll, delta_time * pitch, delta_time * yaw);
        }
        if (dot(offset, offset) != 0)
        {
            AddRelativeOffset(move_speed_ * offset * delta_time * ((flags_ & Accelerate) ? accelerated_speed_ : move_speed_));
        }
        bool updated = update_matrices_;
        UpdateMatrices();
        if (update_matrices_)
        {
            registry_->patch<components::Camera>(camera_, [&](components::Camera &cam) { cam = local_camera_; });
            registry_->patch<components::Transform>(camera_, [&](components::Transform& transform) { transform = local_transform_; });
        }
        camera_update_observer.clear();
        flags_ = None;
    }
} // namespace lighten