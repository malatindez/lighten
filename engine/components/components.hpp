#pragma once
#include "include/library-pch.hpp"
namespace engine::components
{
#pragma warning(push)
#pragma warning(disable : 26495)
    struct GameObject final
    {
        GameObject() = default;
        GameObject(const GameObject &) = default;
        GameObject(GameObject &&) = default;
        GameObject &operator=(const GameObject &) = default;
        GameObject &operator=(GameObject &&) = default;
        ~GameObject() = default;
        std::string name;
        std::string tag;
        // TODO
        // class Entity that will have pointer to entt::registry and will take care of
        // updating GameObject and handling components
        //        entt::entity parent;
        //        std::vector<entt::entity> children;
        //        std::vector<std::string> components;
    };
    struct TransformComponent final
    {
        TransformComponent() = default;
        TransformComponent(const TransformComponent &) = default;
        TransformComponent(TransformComponent &&) = default;
        TransformComponent &operator=(const TransformComponent &) = default;
        TransformComponent &operator=(TransformComponent &&) = default;
        ~TransformComponent() = default;
        void reset()
        {
            position = core::math::vec3{ 0 };
            scale = core::math::vec3{ 1 };
            rotation = core::math::quat{ 1, 0, 0, 0 };
            UpdateMatrices();
        }
        void UpdateMatrices() noexcept
        {
            model = core::math::mat4{ 1 };
            model = core::math::translate(model, position);
            model = model * rotation.as_mat4();
            model = core::math::scale(model, scale);
            inv_model = core::math::inverse(model);
        }
        core::math::vec3 position{ 0 };
        float padding0 = 0;
        core::math::vec3 scale{ 1 };
        float padding1 = 0;
        core::math::quat rotation;
        core::math::mat4 model;
        core::math::mat4 inv_model;
    };
#pragma warning(pop)

    struct CameraComponent final
    {
        CameraComponent() = default;
        CameraComponent(const CameraComponent &) = default;
        CameraComponent(CameraComponent &&) = default;
        CameraComponent &operator=(const CameraComponent &) = default;
        CameraComponent &operator=(CameraComponent &&) = default;
        ~CameraComponent() = default;

        constexpr core::math::vec3 right() const noexcept
        {
            return core::math::vec3{ inv_view[0][0], inv_view[0][1], inv_view[0][2] };
        }
        constexpr core::math::vec3 up() const noexcept
        {
            return core::math::vec3{ inv_view[1][0], inv_view[1][1], inv_view[1][2] };
        }
        constexpr core::math::vec3 forward() const noexcept
        {
            return core::math::vec3{ inv_view[2][0], inv_view[2][1], inv_view[2][2] };
        }
        constexpr core::math::vec3 position() const noexcept
        {
            return core::math::vec3{ inv_view[3][0], inv_view[3][1], inv_view[3][2] };
        }

        core::math::mat4 view{ 1 };
        core::math::mat4 projection{ 1 };
        core::math::mat4 view_projection{ 1 };
        core::math::mat4 inv_view{ 1 };
        core::math::mat4 inv_projection{ 1 };
        core::math::mat4 inv_view_projection{ 1 };
        float fovy_ = core::math::radians(45.0f);
        float z_near_ = 20000.0f;
        float z_far_ = 0.002f;
    };
    struct PointLight final
    {
        PointLight() = default;
        PointLight(const PointLight &) = default;
        PointLight(PointLight &&) = default;
        PointLight &operator=(const PointLight &) = default;
        PointLight &operator=(PointLight &&) = default;
        ~PointLight() = default;

        core::math::vec3 color;
        float power;
    };
    struct DirectionalLight final
    {
        DirectionalLight() = default;
        DirectionalLight(const DirectionalLight &) = default;
        DirectionalLight(DirectionalLight &&) = default;
        DirectionalLight &operator=(const DirectionalLight &) = default;
        DirectionalLight &operator=(DirectionalLight &&) = default;
        ~DirectionalLight() = default;
        // direction is determined by transform
        float solid_angle;
        core::math::vec3 color;
        float power;
    };
    struct SpotLight final
    {
        SpotLight() = default;
        SpotLight(const SpotLight &) = default;
        SpotLight(SpotLight &&) = default;
        SpotLight &operator=(const SpotLight &) = default;
        SpotLight &operator=(SpotLight &&) = default;
        ~SpotLight() = default;

        core::math::vec3 color;
        float power;
        core::math::vec3 direction; // should be normalized
        float cut_off;
    };
} // namespace engine::components