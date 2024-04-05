#pragma once
#include "imgui-layer.hpp"
#include "layer-stack.hpp"
#include "../scene.hpp"
namespace lighten::core
{
    class RenderPipeline : public LayerStack
    {
    public:
        RenderPipeline(std::string_view pipeline_name) : LayerStack(pipeline_name) {}
        void OnRender() = 0;
    protected:
        void InitImGuiLayer(std::shared_ptr<core::Window> window)
        {
            imgui_layer_ = std::make_shared<ImGuiLayer>(window);
            PushOverlay(imgui_layer_);
        }
        virtual void FrameBegin() = 0;
        virtual void FrameEnd() = 0;
        std::shared_ptr<ImGuiLayer> imgui_layer_ = nullptr;
    };
} // namespace lighten::core