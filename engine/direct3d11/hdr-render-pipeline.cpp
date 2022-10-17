#include "hdr-render-pipeline.hpp"
#include "components/components.hpp"
#include "subsystems/render/skybox-manager.hpp"
#include "subsystems/core/input-layer.hpp"
namespace engine::direct3d
{
    HDRRenderPipeline::HDRRenderPipeline(std::shared_ptr<core::Window> window, std::shared_ptr<SwapchainRenderTarget> const &output_target)
        : core::RenderPipeline(),
        hdr_target_{ DXGI_FORMAT_R16G16B16A16_FLOAT },
        output_target_{ output_target }
    {
        viewport_.MinDepth = 0.0f;
        viewport_.MaxDepth = 1.0f;
        viewport_.TopLeftX = 0;
        viewport_.TopLeftY = 0;
        hdr_target_.init();
        InitImGuiLayer(window);
        post_processing_ = std::make_shared<render::PostProcessing>();
        hdr_to_ldr_layer_ = std::make_shared<render::HDRtoLDRLayer>(*output_target_);
        post_processing_->AddLayer(hdr_to_ldr_layer_);
        this->PushLayer(post_processing_);
        window_ = window;
    }
    void HDRRenderPipeline::WindowSizeChanged(core::math::ivec2 const &size)
    {
        hdr_target_.SizeResources(size);
        output_target_->SizeResources(size);
        D3D11_TEXTURE2D_DESC desc = output_target_->render_target_description();
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        desc.Width = static_cast<uint32_t>(size.x);
        desc.Height = static_cast<uint32_t>(size.y);
        desc.SampleDesc.Count = hdr_target_.render_target_description().SampleDesc.Count;
        desc.SampleDesc.Quality = hdr_target_.render_target_description().SampleDesc.Quality;
        depth_stencil_.init(&desc, nullptr);

        // Set up the viewport.
        viewport_.Width = (float)size.x;
        viewport_.Height = (float)size.y;
        direct3d::api().devcon4->RSSetViewports(1, &viewport_);
    }
    void HDRRenderPipeline::OnRender()
    {
        FrameBegin();
        scene_->Render();
        for (auto entity : scene_->registry.view<components::SkyboxComponent>())
        {
            SkyboxManager::RenderSkybox(scene_->registry.get<components::SkyboxComponent>(entity), per_frame_);
        }
        // Render frame
        LayerStack::OnRender();

        PostProcess();
        imgui_layer_->Begin();
        OnGuiRender();
        imgui_layer_->End();
        // End frame
        FrameEnd();
    }
    void HDRRenderPipeline::OnUpdate()
    {
        core::RenderPipeline::OnUpdate();
    }
    void HDRRenderPipeline::OnEvent(core::events::Event &e)
    {
        if (e.type() == core::events::EventType::WindowResize)
        {
            auto &event = static_cast<core::events::WindowResizeEvent &>(e);
            WindowSizeChanged(event.size());
        }
        core::RenderPipeline::OnEvent(e);
    }

    void HDRRenderPipeline::FrameBegin()
    {
        api().devcon4->RSSetViewports(1, &viewport_);
        api().devcon4->OMSetRenderTargets(1, &hdr_target_.render_target_view(), depth_stencil_.depth_stencil_view());

        api().devcon4->ClearRenderTargetView(hdr_target_.render_target_view(), sky_color_.data.data());
        api().devcon4->ClearDepthStencilView(depth_stencil_.depth_stencil_view(), D3D11_CLEAR_DEPTH, 0.0f, 0);

        auto const &camera = scene_->main_camera->camera();
        per_frame_.view = camera.view;
        per_frame_.projection = camera.projection;
        per_frame_.view_projection = camera.view_projection;
        per_frame_.inv_view = camera.inv_view;
        per_frame_.inv_projection = camera.inv_projection;
        per_frame_.inv_view_projection = camera.inv_view_projection;
        per_frame_.screen_resolution = core::math::vec2{ viewport_.Width, viewport_.Height };

        per_frame_.mouse_position = core::math::vec2{ core::InputLayer::instance()->mouse_position() };

        per_frame_buffer_.Update(per_frame_);
        per_frame_buffer_.Bind(ShaderType::VertexShader, 0);
        per_frame_buffer_.Bind(ShaderType::PixelShader, 0);
        post_processing_->OnFrameBegin();
    }
    void HDRRenderPipeline::PostProcess()
    {
        post_processing_->OnProcess(hdr_target_);
    }
    void HDRRenderPipeline::FrameEnd()
    {
        post_processing_->OnFrameEnd(*output_target_);
    }
}