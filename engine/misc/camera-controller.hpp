#pragma once
#include "components/camera.hpp"
#include "components/transform.hpp"
#include "entt/entt.hpp"
#include <iostream>
namespace engine
{
    class CameraController
    {
    public:
        CameraController(components::Camera &camera, components::Transform &transform, math::ivec2 const &size) : camera_(camera), transform_(transform), size_(size)
        {
            SetProjectionMatrix(
                math::perspective(
                    camera.fovy_, 
                    static_cast<float>(size_.x) / size_.y, 
                    camera.z_far_,
                     camera.z_near_));
            UpdateMatrices();
        }

        void SetProjectionMatrix(math::mat4 const& proj) 
        { 
            camera_.projection = proj; 
            camera_.inv_projection = math::inverse(proj); 
        }

        void SetWorldOffset(math::vec3 const &offset);
        void AddWorldOffset(math::vec3 const &offset);
        void AddRelativeOffset(math::vec3 const &offset);

        void SetWorldAngles(math::vec3 const &angles);
        void AddWorldAngles(math::vec3 const &angles);
        void AddRelativeAngles(math::vec3 const &angles);
        
        void UpdateBasis();

        void UpdateMatrices();

        [[nodiscard]] constexpr components::Camera const &camera() const noexcept { return camera_; }
        [[nodiscard]] constexpr components::Transform const &transform() const noexcept { return transform_; }
        [[nodiscard]] constexpr math::ivec2 const &size() const noexcept { return size_; }

        [[nodiscard]] constexpr math::vec3 const &position() const noexcept { return transform_.position; }
        [[nodiscard]] constexpr math::vec3 const &scale() const noexcept { return transform_.scale; }
        [[nodiscard]] constexpr math::quat const &rotation() const noexcept { return transform_.rotation; }

        [[nodiscard]] constexpr math::vec3 right() const noexcept { return right_; }
        [[nodiscard]] constexpr math::vec3 up() const noexcept { return up_; }
        [[nodiscard]] constexpr math::vec3 forward() const noexcept { return forward_; }

        [[nodiscard]] constexpr float fovy() const noexcept { return camera_.fovy_; }
        [[nodiscard]] constexpr float z_near() const noexcept { return camera_.z_near_; }
        [[nodiscard]] constexpr float z_far() const noexcept { return camera_.z_far_; }

        static constexpr math::vec3 kWorldUp{0, 1, 0};

    private:
        components::Camera &camera_;
        components::Transform &transform_;
        math::ivec2 const &size_;

        math::vec3 right_{1, 0, 0};
        math::vec3 up_{0, 1, 0};
        math::vec3 forward_{0, 0, 1};


        bool update_matrices_ = true;
        bool update_basis_ = true;
    };
}