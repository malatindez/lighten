#pragma once
#include "components/components.hpp"
#include "core/math/ray.hpp"
namespace engine
{

    class CameraController
    {
    public:
        enum Flags
        {
            None = 0,
            MoveForward = 1 << 0,
            MoveBackwards = 1 << 1,
            MoveRight = 1 << 2,
            MoveLeft = 1 << 3,
            MoveUp = 1 << 4,
            MoveDown = 1 << 5,
            RotateLeft = 1 << 6,
            RotateRight = 1 << 7,
            Accelerate = 1 << 8
        };

        CameraController(components::CameraComponent *camera, components::TransformComponent *transform,
                         core::math::ivec2 const &window_size);

        void Init();
        void UpdateProjectionMatrix();

        void SetProjectionMatrix(core::math::mat4 const &proj);

        void SetWorldOffset(core::math::vec3 const &offset);
        void AddWorldOffset(core::math::vec3 const &offset);
        void AddRelativeOffset(core::math::vec3 const &offset);

        void SetWorldAngles(float roll, float pitch, float yaw);
        void AddWorldAngles(float roll, float pitch, float yaw);
        void AddRelativeAngles(float roll, float pitch, float yaw);

        void UpdateBasis();

        void UpdateMatrices();

        // ndc in range from -1 to 1
        // returns a projected ray from camera
        [[nodiscard]] inline core::math::Ray Raycast(core::math::vec2 const &ndc) const noexcept
        {
            core::math::vec4 direction = core::math::vec4(ndc.x, ndc.y, 1, 1) * camera_->inv_view_projection;
            return core::math::Ray(camera_->position(), core::math::normalize(as_vec<3>(direction) / direction.w - camera_->position()));
        }

        [[nodiscard]] constexpr bool &roll_enabled() noexcept { return roll_enabled_; }
        [[nodiscard]] constexpr bool const &roll_enabled() const noexcept { return roll_enabled_; }

        [[nodiscard]] constexpr components::CameraComponent const &camera() const noexcept { return *camera_; }
        [[nodiscard]] constexpr components::TransformComponent const &transform() const noexcept { return *transform_; }
        [[nodiscard]] constexpr components::CameraComponent &camera() noexcept { return *camera_; }
        [[nodiscard]] constexpr components::TransformComponent &transform() noexcept { return *transform_; }

        [[nodiscard]] constexpr core::math::ivec2 const &window_size() const noexcept { return window_size_; }
        [[nodiscard]] constexpr core::math::vec3 const &position() const noexcept { return transform_->position; }
        [[nodiscard]] constexpr core::math::quat const &rotation() const noexcept { return transform_->rotation; }

        [[nodiscard]] constexpr core::math::vec3 const &right() const noexcept { return core::math::as_crvec<3>(camera_->inv_view[0]); }
        [[nodiscard]] constexpr core::math::vec3 const &up() const noexcept { return core::math::as_crvec<3>(camera_->inv_view[1]); }
        [[nodiscard]] constexpr core::math::vec3 const &forward() const noexcept { return core::math::as_crvec<3>(camera_->inv_view[2]); }

        [[nodiscard]] constexpr float fovy() const noexcept { return camera_->fovy_; }
        [[nodiscard]] constexpr float z_near() const noexcept { return camera_->z_near_; }
        [[nodiscard]] constexpr float z_far() const noexcept { return camera_->z_far_; }

        [[nodiscard]] constexpr uint32_t &flags() noexcept { return flags_; }

        [[nodiscard]] float &sensivity() noexcept { return sensivity_; }
        [[nodiscard]] float const &sensivity() const noexcept { return sensivity_; }
        [[nodiscard]] float &move_speed() noexcept { return move_speed_; }
        [[nodiscard]] float const &move_speed() const noexcept { return move_speed_; }
        [[nodiscard]] float &accelerated_speed() noexcept { return accelerated_speed_; }
        [[nodiscard]] float const &accelerated_speed() const noexcept { return accelerated_speed_; }
        [[nodiscard]] float &roll_speed() noexcept { return roll_speed_; }
        [[nodiscard]] float const &roll_speed() const noexcept { return roll_speed_; }
        void SetNewCamera(components::CameraComponent *camera, components::TransformComponent *transform) noexcept
        {
            camera_ = camera;
            transform_ = transform;
            Init();
        }
        void OnTick(float delta_time, core::math::ivec2 const &pixel_mouse_delta);

    private:
        uint32_t flags_ = None;
        float sensivity_ = 8.0f;
        float move_speed_ = 2.0f;
        float accelerated_speed_ = 10.0f;
        float roll_speed_ = engine::core::math::radians(60.0f);

    private:
        components::CameraComponent *camera_;
        components::TransformComponent *transform_;
        core::math::ivec2 const &window_size_;

        bool update_matrices_ = true;
        bool update_basis_ = true;
        bool roll_enabled_ = false;
    };
} // namespace engine