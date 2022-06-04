#pragma once

#include "components/camera.hpp"
#include "components/directional-light.hpp"
#include "components/mesh-component.hpp"
#include "components/point-light.hpp"
#include "components/sphere.hpp"
#include "components/spot-light.hpp"
#include "core/bitmap-window.hpp"
#include "core/parallel-executor.hpp"
#include "entt/entt.hpp"
#include "math/ray.hpp"
#include "render/floor.hpp"
namespace engine
{
    class Scene
    {
    public:
        Scene() = default;
        void UpdateScene() noexcept;

        void Draw(components::Camera const &cam, core::BitmapWindow &window, core::ParallelExecutor &executor);

        [[nodiscard]] std::optional<entt::entity> GetIntersectedEntity(core::math::Intersection &intersection, core::math::Ray &ray);

        std::optional<entt::entity> GetIntersectedEntityIf(
            core::math::Intersection &intersection, core::math::Ray &ray,
            std::function<bool(entt::entity, components::Transform const &, render::Material const &)> const &func);
        bool update_scene{true};

        entt::registry registry;
        render::Floor floor;
    };
} // namespace engine