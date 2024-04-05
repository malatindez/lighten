#pragma once
#include "../core/layers/layer.hpp"
#include "../include/imgui.hpp"
#include "../direct3d11/direct3d11.hpp"
#include "../render/abstract/viewport-base.hpp"
#include "ui-helpers.hpp"

namespace lighten::gui
{
    class UI_ViewportRenderer : public core::LayerStack
    {
    public:
        UI_ViewportRenderer(std::shared_ptr<render::ViewportBase> rendering_viewport) : core::LayerStack("UI_ViewportRenderer"), rendering_viewport_{rendering_viewport}
        {
            PushLayer(rendering_viewport_);
        }
        
        void OnGuiRender() override;
        void DisplayViewport();
        
        [[nodiscard]] render::ViewportBase *rendering_viewport() const noexcept { return rendering_viewport_.get(); }

        private:
            std::shared_ptr<render::ViewportBase> rendering_viewport_;

            glm::ivec2 framebuffer_pos_ = glm::ivec2{ 0 };
            glm::ivec2 framebuffer_size_ = glm::ivec2{ 0 };
    };
}