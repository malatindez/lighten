#pragma once
#include "components/components.hpp"
namespace lighten
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

        CameraController(entt::registry *registry, entt::entity camera, glm::ivec2 const &window_size);

        void Init();
        void UpdateProjectionMatrix();

        void SetProjectionMatrix(glm::mat4 const &proj);

        void SetWorldOffset(glm::vec3 const &offset);
        void AddWorldOffset(glm::vec3 const &offset);
        void AddRelativeOffset(glm::vec3 const &offset);

        void SetWorldAngles(float roll, float pitch, float yaw);
        void AddWorldAngles(float roll, float pitch, float yaw);
        void AddRelativeAngles(float roll, float pitch, float yaw);

        void UpdateBasis();

        void UpdateMatrices();

        // ndc in range from -1 to 1
        // returns a projected ray from camera
        [[nodiscard]] inline core::math::Ray Raycast(glm::vec2 const &ndc) const noexcept
        {
            glm::vec4 direction = local_camera_.inv_view_projection * glm::vec4(ndc.x, ndc.y, 1, 1);
            return core::math::Ray(position(), glm::normalize(glm::vec3{direction} / direction.w - position()));
        }

        [[nodiscard]] inline core::math::Ray PixelRaycast(glm::vec2 ndc) const noexcept
        {
            ndc /= window_size();
            ndc = ndc * 2.0f - 1.0f;
            ndc.y = -ndc.y;
            return Raycast(ndc);
        }
        [[nodiscard]] inline entt::entity get_camera()                  const noexcept { return camera_; }
        [[nodiscard]] inline components::Camera get_last_camera_state() const noexcept { return local_camera_; }

        [[nodiscard]] constexpr glm::ivec2 const &window_size()         const noexcept { return window_size_; }
        [[nodiscard]] inline glm::quat const &rotation()                const noexcept { return local_transform_.rotation; }

        [[nodiscard]] inline glm::vec3 right()                          const noexcept { return local_camera_.right(); }
        [[nodiscard]] inline glm::vec3 up()                             const noexcept { return local_camera_.up(); }
        [[nodiscard]] inline glm::vec3 forward()                        const noexcept { return local_camera_.forward(); }
        [[nodiscard]] inline glm::vec3 position()                       const noexcept { return local_camera_.position(); }

        [[nodiscard]] inline float const& fovy()                        const noexcept { return local_camera_.fovy_; }
        [[nodiscard]] inline float const& z_near()                      const noexcept { return local_camera_.z_near_; }
        [[nodiscard]] inline float const& z_far()                       const noexcept { return local_camera_.z_far_; }

        [[nodiscard]] inline float &fovy()                              noexcept       { return local_camera_.fovy_; }
        [[nodiscard]] inline float &z_near()                            noexcept       { return local_camera_.z_near_; }
        [[nodiscard]] inline float &z_far()                             noexcept       { return local_camera_.z_far_; }

        [[nodiscard]] constexpr uint32_t &flags()                       noexcept       { return flags_; }

        [[nodiscard]] constexpr bool const &roll_enabled()              const noexcept { return roll_enabled_; }
        [[nodiscard]] constexpr float const &sensitivity()              const noexcept { return sensitivity_; }
        [[nodiscard]] constexpr float const &move_speed()               const noexcept { return move_speed_; }
        [[nodiscard]] constexpr float const &accelerated_speed()        const noexcept { return accelerated_speed_; }
        [[nodiscard]] constexpr float const &roll_speed()               const noexcept { return roll_speed_; }

        [[nodiscard]] constexpr bool &roll_enabled()                    noexcept       { return roll_enabled_; }
        [[nodiscard]] constexpr float &sensitivity()                    noexcept       { return sensitivity_; }
        [[nodiscard]] constexpr float &move_speed()                     noexcept       { return move_speed_; }
        [[nodiscard]] constexpr float &accelerated_speed()              noexcept       { return accelerated_speed_; }
        [[nodiscard]] constexpr float &roll_speed()                     noexcept       { return roll_speed_; }
        
        void SetNewCamera(entt::registry *registry, entt::entity new_camera) noexcept
        {
            registry_ = registry;
            camera_ = new_camera;
            Init();
        }
        void OnTick(float delta_time, glm::ivec2 const &pixel_mouse_delta);

    private:
        entt::observer camera_update_observer;
        uint32_t flags_ = None;
        float sensitivity_ = 12.0f;
        float move_speed_ = 2.0f;
        float accelerated_speed_ = 10.0f;
        float roll_speed_ = glm::radians(60.0f);

        components::Camera local_camera_;
        components::Transform local_transform_;

    public:
        entt::registry *registry_;
        entt::entity camera_;
        glm::ivec2 const &window_size_;

        bool update_matrices_ = true;
        bool update_basis_ = true;
        bool roll_enabled_ = false;
    };
} // namespace lighten