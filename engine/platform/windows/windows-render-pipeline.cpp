#include "windows-render-pipeline.hpp"
#include "core/engine.hpp"
using namespace engine::direct3d;
namespace engine::platform::windows
{
    RenderPipeline::RenderPipeline() : core::RenderPipeline()
    {
        window_ = std::make_shared<Window>();

        InitImGuiLayer(std::static_pointer_cast<core::Window>(window_));
    }

    void RenderPipeline::OnRender()
    {
        if (!window_->alive()) [[unlikely]]
            return;
        FrameBegin();
        // Render frame
        LayerStack::OnRender();
        imgui_layer_->Begin();
        OnGuiRender();
        imgui_layer_->End();
        // End frame
        FrameEnd();
    }
    void RenderPipeline::OnUpdate()
    {
        window_->OnUpdate();
        core::RenderPipeline::OnUpdate();
    }

    void RenderPipeline::FrameBegin()
    {
        ID3D11RenderTargetView *view = window_->frame_buffer_view();
        api::devcon4->OMSetRenderTargets(1, &view, window_->depth_buffer_view());

        direct3d::api::devcon4->ClearRenderTargetView(window_->frame_buffer_view(), sky_color_.data.data());
        direct3d::api::devcon4->ClearDepthStencilView(window_->depth_buffer_view(), D3D11_CLEAR_DEPTH, 0.0f, 0);
    }
    void RenderPipeline::FrameEnd()
    {

        // switch the back buffer and the front buffer
        window_->swapchain()->Present(1, 0);
    }

}