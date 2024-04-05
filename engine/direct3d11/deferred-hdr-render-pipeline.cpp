#include "deferred-hdr-render-pipeline.hpp"
#include "components/components.hpp"
#include "subsystems/core/input-layer.hpp"
#include "subsystems/render/skybox-manager.hpp"
#include "core/engine.hpp"

#include "imgui_internal.h"  
namespace lighten::direct3d
{
    DeferredHDRRenderPipeline::DeferredHDRRenderPipeline(std::shared_ptr<core::Window> window, std::shared_ptr<SwapchainRenderTarget> const &output_target)
        : core::RenderPipeline("Deferred HDR Render Pipeline"),
        output_target_{output_target}
    {
        viewport_.MinDepth = 0.0f;
        viewport_.MaxDepth = 1.0f;
        viewport_.TopLeftX = 0;
        viewport_.TopLeftY = 0;
        InitImGuiLayer(window);
        window_ = window;
        framebuffer_size_ = glm::min(window->size(), glm::ivec2(32, 32));
        imgui_layer_->BlockEvents(false);
    }
    void DeferredHDRRenderPipeline::WindowSizeChanged(glm::ivec2 const &input_size)
    {
        if (framebuffer_size_.x <= 1 || framebuffer_size_.y <= 1)
        {
            framebuffer_size_ = input_size;
        }
        auto const& size = framebuffer_size_;

        output_target_->SizeResources(size);
    }
    void DeferredHDRRenderPipeline::OnRender()
    {
        FrameBegin(); 
        
        direct3d::api().devcon4->PSSetSamplers(0, 1, &direct3d::states().bilinear_wrap_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(1, 1, &direct3d::states().bilinear_clamp_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(2, 1, &direct3d::states().anisotropic_wrap_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(3, 1, &direct3d::states().anisotropic_clamp_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(4, 1, &direct3d::states().point_wrap_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(5, 1, &direct3d::states().point_clamp_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(6, 1, &direct3d::states().comparison_linear_clamp_sampler.ptr());

        direct3d::api().devcon4->VSSetSamplers(0, 1, &direct3d::states().bilinear_wrap_sampler.ptr());
        direct3d::api().devcon4->VSSetSamplers(1, 1, &direct3d::states().anisotropic_wrap_sampler.ptr());
        direct3d::api().devcon4->VSSetSamplers(2, 1, &direct3d::states().bilinear_clamp_sampler.ptr());
        direct3d::api().devcon4->VSSetSamplers(3, 1, &direct3d::states().comparison_linear_clamp_sampler.ptr());
        direct3d::api().devcon4->VSSetSamplers(4, 1, &direct3d::states().point_wrap_sampler.ptr());
        direct3d::api().devcon4->VSSetSamplers(5, 1, &direct3d::states().point_clamp_sampler.ptr());
        direct3d::api().devcon4->VSSetSamplers(6, 1, &direct3d::states().comparison_linear_clamp_sampler.ptr());

        direct3d::api().devcon4->CSSetSamplers(0, 1, &direct3d::states().bilinear_wrap_sampler.ptr());
        direct3d::api().devcon4->CSSetSamplers(1, 1, &direct3d::states().anisotropic_wrap_sampler.ptr());
        direct3d::api().devcon4->CSSetSamplers(2, 1, &direct3d::states().bilinear_clamp_sampler.ptr());
        direct3d::api().devcon4->CSSetSamplers(3, 1, &direct3d::states().comparison_linear_clamp_sampler.ptr());
        direct3d::api().devcon4->CSSetSamplers(4, 1, &direct3d::states().point_wrap_sampler.ptr());
        direct3d::api().devcon4->CSSetSamplers(5, 1, &direct3d::states().point_clamp_sampler.ptr());
        direct3d::api().devcon4->CSSetSamplers(6, 1, &direct3d::states().comparison_linear_clamp_sampler.ptr());

        core::RenderPipeline::OnRender();

        const glm::vec4 kCleanColor{0.0f, 0.0f, 0.0f, 1.0f};
        api().devcon4->ClearRenderTargetView(output_target_->render_target_view(), reinterpret_cast<const float*>(&kCleanColor));
        api().devcon4->OMSetRenderTargets(1, &output_target_->render_target_view(), direct3d::null_dsv);


        imgui_layer_->Begin();
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("Main Dockspace", nullptr, window_flags);
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_AutoHideTabBar);
        ImGui::End();

        
        OnGuiRender();

        imgui_layer_->End();
        // End frame
        FrameEnd();
    }
    void DeferredHDRRenderPipeline::OnEvent(core::events::Event &e)
    {
        if (e.type() == core::events::EventType::WindowResize)
        {
            auto &event = static_cast<core::events::WindowResizeEvent &>(e);
            WindowSizeChanged(event.size());
        }
        core::RenderPipeline::OnEvent(e);
    }

    void DeferredHDRRenderPipeline::FrameBegin()
    {
    }
    void DeferredHDRRenderPipeline::PostProcess()
    {
    }
    void DeferredHDRRenderPipeline::FrameEnd()
    {
        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        output_target_->swapchain()->Present(0, 0);
    }
} // namespace lighten::direct3d