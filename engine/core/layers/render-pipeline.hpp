#pragma once
#include "imgui-layer.hpp"
#include "layer-stack.hpp"
#include "../scene.hpp"
namespace engine::core
{
    class RenderPipeline : public LayerStack
    {
    public:
        RenderPipeline() : LayerStack("RenderPipeline") {}
        void OnRender() = 0;
        void SetScene(std::shared_ptr<Scene> const &scene) { scene_ = scene; }
        [[nodiscard]] inline std::shared_ptr<Scene> const &scene() const noexcept { return scene_; }
        [[nodiscard]] inline std::shared_ptr<Scene> &scene() noexcept { return scene_; }
    protected:
        void InitImGuiLayer(std::shared_ptr<core::Window> window)
        {
            imgui_layer_ = std::make_shared<ImGuiLayer>(window);
            PushOverlay(imgui_layer_);
        }
        virtual void FrameBegin() = 0;
        virtual void FrameEnd() = 0;
        std::shared_ptr<Scene> scene_ = nullptr;
        std::shared_ptr<ImGuiLayer> imgui_layer_ = nullptr;
    };
} // namespace engine::core