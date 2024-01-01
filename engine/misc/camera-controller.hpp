#pragma once
#include "components/components.hpp"
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

        CameraController(entt::registry *registry, entt::entity camera, core::math::ivec2 const &window_size);

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
            auto const &cam = camera();
            core::math::vec4 direction = cam.inv_view_projection * core::math::vec4(ndc.x, ndc.y, 1, 1);
            return core::math::Ray(cam.position(), core::math::normalize(glm::vec3{ direction } / direction.w - cam.position()));
        }

        [[nodiscard]] inline core::math::Ray PixelRaycast(core::math::vec2 ndc) const noexcept
        {
            ndc /= window_size();
            ndc = ndc * 2.0f - 1.0f;
            ndc.y = -ndc.y;
            return Raycast(ndc);
        }

        [[nodiscard]] inline components::CameraComponent const &camera() const noexcept { return registry_->get<components::CameraComponent>(camera_); }
        [[nodiscard]] inline components::Transform const &transform() const noexcept { return registry_->get<components::Transform>(camera_); }
        [[nodiscard]] inline components::CameraComponent &camera() noexcept { return registry_->get<components::CameraComponent>(camera_); }
        [[nodiscard]] inline components::Transform &transform() noexcept { return registry_->get<components::Transform>(camera_); }

        [[nodiscard]] constexpr core::math::ivec2 const &window_size() const noexcept { return window_size_; }
        [[nodiscard]] inline core::math::vec3 const &position() const noexcept { return transform().position; }
        [[nodiscard]] inline core::math::quat const &rotation() const noexcept { return transform().rotation; }

        [[nodiscard]] inline core::math::vec3 right() const noexcept { return camera().inv_view[0]; }
        [[nodiscard]] inline core::math::vec3 up() const noexcept { return camera().inv_view[1]; }
        [[nodiscard]] inline core::math::vec3 forward() const noexcept { return camera().inv_view[2]; }

        [[nodiscard]] inline float fovy() const noexcept { return camera().fovy_; }
        [[nodiscard]] inline float z_near() const noexcept { return camera().z_near_; }
        [[nodiscard]] inline float z_far() const noexcept { return camera().z_far_; }

        [[nodiscard]] constexpr uint32_t &flags() noexcept { return flags_; }

        [[nodiscard]] constexpr bool &roll_enabled() noexcept { return roll_enabled_; }
        [[nodiscard]] constexpr bool const &roll_enabled() const noexcept { return roll_enabled_; }
        [[nodiscard]] constexpr float &sensivity() noexcept { return sensivity_; }
        [[nodiscard]] constexpr float const &sensivity() const noexcept { return sensivity_; }
        [[nodiscard]] constexpr float &move_speed() noexcept { return move_speed_; }
        [[nodiscard]] constexpr float const &move_speed() const noexcept { return move_speed_; }
        [[nodiscard]] constexpr float &accelerated_speed() noexcept { return accelerated_speed_; }
        [[nodiscard]] constexpr float const &accelerated_speed() const noexcept { return accelerated_speed_; }
        [[nodiscard]] constexpr float &roll_speed() noexcept { return roll_speed_; }
        [[nodiscard]] constexpr float const &roll_speed() const noexcept { return roll_speed_; }
        void SetNewCamera(entt::registry *registry, entt::entity new_camera) noexcept
        {
            registry_ = registry;
            camera_ = new_camera;
            Init();
        }
        void OnTick(float delta_time, core::math::ivec2 const &pixel_mouse_delta);

    private:
        uint32_t flags_ = None;
        float sensivity_ = 8.0f;
        float move_speed_ = 2.0f;
        float accelerated_speed_ = 10.0f;
        float roll_speed_ = engine::core::math::radians(60.0f);

    public:
        entt::registry *registry_;
        entt::entity camera_;
        core::math::ivec2 const &window_size_;

        bool update_matrices_ = true;
        bool update_basis_ = true;
        bool roll_enabled_ = false;
    };
} // namespace engine