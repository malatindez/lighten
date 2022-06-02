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

    // uv in range from -1 to 1
    // returns a projected ray from camera
    [[nodiscard]] inline math::Ray Raycast(math::vec2 const &uv) const noexcept
    {
      math::vec4 bl4 = math::vec4(-1, -1, 1, 1) * camera_.inv_view_projection;
      math::vec4 br4 = math::vec4(1, -1, 1, 1) * camera_.inv_view_projection;
      math::vec4 tl4 = math::vec4(-1, 1, 1, 1) * camera_.inv_view_projection;
      math::vec4 t = (bl4 + (tl4 - bl4) * uv.v + (br4 - bl4) * uv.u);
      return math::Ray(camera_.position(), math::normalize(t.as_vec<3>() / t.w - camera_.position()));
    }

    [[nodiscard]] constexpr components::Camera const &camera() const noexcept { return camera_; }
    [[nodiscard]] constexpr components::Transform const &transform() const noexcept { return transform_; }
    [[nodiscard]] constexpr math::ivec2 const &size() const noexcept { return window_size_; }

    [[nodiscard]] constexpr math::vec3 const &position() const noexcept { return transform_.position; }
    [[nodiscard]] constexpr math::vec3 const &scale() const noexcept { return transform_.scale; }
    [[nodiscard]] constexpr math::quat const &rotation() const noexcept { return transform_.rotation; }

    [[nodiscard]] constexpr math::vec3 right() const noexcept { return right_; }
    [[nodiscard]] constexpr math::vec3 up() const noexcept { return up_; }
    [[nodiscard]] constexpr math::vec3 forward() const noexcept
    {
      return forward_;
    }

    [[nodiscard]] constexpr float fovy() const noexcept { return camera_.fovy_; }
    [[nodiscard]] constexpr float z_near() const noexcept
    {
      return camera_.z_near_;
    }
    [[nodiscard]] constexpr float z_far() const noexcept
    {
      return camera_.z_far_;
    }

  private:
    components::Camera &camera_;
    components::Transform &transform_;
    math::ivec2 const &window_size_;

    math::vec3 right_{1, 0, 0};
    math::vec3 up_{0, 1, 0};
    math::vec3 forward_{0, 0, 1};

    bool update_matrices_ = true;
    bool update_basis_ = true;
  };
} // namespace engine