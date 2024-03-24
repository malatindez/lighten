#pragma once
#include "include/library-pch.hpp"
#include "components/transform.hpp"
#include "components/game-object.hpp"
#include "core/layers/layer.hpp"

namespace lighten::systems
{
    LIGHTEN_SYSTEM(name = "Transform Management System", category = "Core")
    struct TransformManagementSystem final : 
        public core::Layer,
        public core::Layer::HandlePreUpdate
    {
        TransformManagementSystem(entt::registry &registry) noexcept;
        using TransformView = entt::basic_view<entt::entity, entt::type_list<components::Transform, components::WorldTransform, components::GameObject>, entt::type_list<>>;

        void OnPreUpdate() noexcept override;
   private:
        entt::observer observer;
        entt::registry &registry;
    };
} // namespace lighten::systems