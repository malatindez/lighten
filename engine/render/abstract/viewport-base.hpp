#pragma once
#include "../../core/layers/layer-stack.hpp"
#include "../../core/engine.hpp"

namespace lighten::render
{
    
    enum class ViewportType : uint32_t
    {
        Deferred = 0,
        Count
    };
    struct ViewportBase : public core::LayerStack
    {
        ViewportBase(std::string name, const ViewportType type) : LayerStack(name), viewport_type_{type} {}
        virtual ~ViewportBase() = default;

        void SetWorld(std::shared_ptr<core::World> world) { world_ = world; }

        // Should have GameObject, Transform and Camera components
        void SetCamera(entt::entity camera) { camera_entity_ = camera; }

        [[nodiscard]] inline std::shared_ptr<core::World> const &world() const noexcept { return world_; }
        [[nodiscard]] inline std::shared_ptr<core::World> &world() noexcept { return world_; }
        [[nodiscard]] inline entt::entity camera() const noexcept { return camera_entity_; }
        virtual void OnUpdate() = 0;
        virtual void OnRender() = 0;
 
        /**
         * @brief Called when the viewport should be resized
         * 
         * @param width 
         * @param height 
         */
        virtual void OnResize(glm::ivec2 const &) = 0;
         
        /**
         * @brief Get the Resource View object, this is the view that should be used to render the scene
         * 
         * @return void* 
         */
        virtual void* GetResourceView() = 0;
        // @brief Get the framebuffer size
        [[nodiscard]] inline glm::ivec2 framebuffer_size() const noexcept { return framebuffer_size_; }
        [[nodiscard]] inline ViewportType viewport_type() const noexcept { return viewport_type_; }

    protected:
        std::shared_ptr<core::World> world_;
        entt::entity camera_entity_ = entt::null;
        glm::ivec2 framebuffer_size_;

        const ViewportType viewport_type_;
    };
}