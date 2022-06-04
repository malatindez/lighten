#pragma once
#include "components/camera.hpp"
#include "components/transform.hpp"
#include "entt/entt.hpp"
#include "math/ray.hpp"
#include <iostream>
namespace engine
{
  class CameraController
  {
  public:
    CameraController(components::Camera &camera, components::Transform &transform,
                     math::ivec2 const &window_size);
    void UpdateProjectionMatrix();

    void SetProjectionMatrix(math::mat4 const &proj);

    void SetWorldOffset(math::vec3 const &offset);
    void AddWorldOffset(math::vec3 const &offset);
    void AddRelativeOffset(math::vec3 const &offset);

    void SetWorldAngles(float roll, float pitch, float yaw);
    void AddWorldAngles(float roll, float pitch, float yaw);
    void AddRelativeAngles(float roll, float pitch, float yaw);

    void UpdateBasis();

    void UpdateMatrices();

    // nfc in range from -1 to 1
    // returns a projected ray from camera
    [[nodiscard]] inline math::Ray Raycast(math::vec2 const &ndc) const noexcept
    {
      math::vec4 direction = math::vec4(ndc.x, ndc.y, 1, 1) * camera_.inv_view_projection;
      return math::Ray(camera_.position(), math::normalize(direction.as_vec<3>() / direction.w - camera_.position()));
    }

    [[nodiscard]] constexpr components::Camera const &camera() const noexcept { return camera_; }
    [[nodiscard]] constexpr components::Transform const &transform() const noexcept { return transform_; }
    [[nodiscard]] constexpr math::ivec2 const &window_size() const noexcept { return window_size_; }

    [[nodiscard]] constexpr math::vec3 const &position() const noexcept { return transform_.position; }
    [[nodiscard]] constexpr math::quat const &rotation() const noexcept { return transform_.rotation; }

    [[nodiscard]] constexpr math::crvec3 right() const noexcept { return math::crvec3{camera_.inv_view[0][0], camera_.inv_view[0][1], camera_.inv_view[0][2]}; }
    [[nodiscard]] constexpr math::crvec3 up() const noexcept { return math::crvec3{camera_.inv_view[1][0], camera_.inv_view[1][1], camera_.inv_view[1][2]}; }
    [[nodiscard]] constexpr math::crvec3 forward() const noexcept { return math::crvec3{camera_.inv_view[2][0], camera_.inv_view[2][1], camera_.inv_view[2][2]}; }

    [[nodiscard]] constexpr float fovy() const noexcept { return camera_.fovy_; }
    [[nodiscard]] constexpr float z_near() const noexcept { return camera_.z_near_; }
    [[nodiscard]] constexpr float z_far() const noexcept { return camera_.z_far_; }

  private:
    components::Camera &camera_;
    components::Transform &transform_;
    math::ivec2 const &window_size_;

    bool update_matrices_ = true;
    bool update_basis_ = true;
  };
} // namespace engine