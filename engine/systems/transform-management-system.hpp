#pragma once
#include "include/library-pch.hpp"
#include "components/transform.hpp"
#include "components/game-object.hpp"
#include "core/layers/layer.hpp"
#include "../core/world.hpp"
namespace lighten::core
{
    class Engine;
}

namespace lighten::systems
{
    LIGHTEN_SYSTEM(name = "Transform Management System", category = "Core")
    struct TransformManagementSystem final : 
        public core::Layer,
        public core::Layer::HandlePreUpdate
    {
        TransformManagementSystem(std::shared_ptr<core::World> world) noexcept;

        void OnPreUpdate() noexcept override;
   private:
        friend class ::lighten::core::Engine;
        using core::Layer::Layer;
        
        entt::observer observer;
        std::shared_ptr<core::World> world_;
    };
} // namespace lighten::systems