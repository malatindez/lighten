#include "deferred-hdr-render-pipeline.hpp"
#include "components/components.hpp"
#include "subsystems/core/input-layer.hpp"
#include "subsystems/render/skybox-manager.hpp"
#include "core/engine.hpp"
#include "core/../../d3d_test/object-editor.hpp"
namespace engine::direct3d
{
    DeferredHDRRenderPipeline::DeferredHDRRenderPipeline(std::shared_ptr<core::Window> window, std::shared_ptr<SwapchainRenderTarget> const &output_target)
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
        hdr_to_ldr_layer_ = std::make_shared<render::HDRtoLDRLayer>(*output_target_);
        window_ = window;

        deferred_resolve_ = std::make_shared<render::DeferredResolve>(hdr_target_);
        hdr_to_ldr_layer_ = std::make_shared<render::HDRtoLDRLayer>(*output_target_);
        swapchain_present_ = std::make_shared<render::PresentSwapchain>();

        gbuffer_.normals = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_SNORM);
        gbuffer_.albedo = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_FLOAT);
        gbuffer_.roughness_metalness_transmittance_ao = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_SNORM);
        gbuffer_.emission = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_FLOAT);
        gbuffer_.entity_id = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R32_UINT);
    }
    void DeferredHDRRenderPipeline::WindowSizeChanged(core::math::ivec2 const &size)
    {
        hdr_target_.SizeResources(size);
        output_target_->SizeResources(size);
        gbuffer_.normals->SizeResources(size);
        gbuffer_.albedo->SizeResources(size);
        gbuffer_.roughness_metalness_transmittance_ao->SizeResources(size);
        gbuffer_.emission->SizeResources(size);
        gbuffer_.entity_id->SizeResources(size);

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
    void DeferredHDRRenderPipeline::OnRender()
    {
        FrameBegin();

        direct3d::api().devcon4->PSSetSamplers(0, 1, &direct3d::states().bilinear_wrap_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(1, 1, &direct3d::states().anisotropic_wrap_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(2, 1, &direct3d::states().bilinear_clamp_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(3, 1, &direct3d::states().comparison_linear_clamp_sampler.ptr());

        direct3d::api().devcon4->VSSetSamplers(0, 1, &direct3d::states().bilinear_wrap_sampler.ptr());
        direct3d::api().devcon4->VSSetSamplers(1, 1, &direct3d::states().anisotropic_wrap_sampler.ptr());
        direct3d::api().devcon4->VSSetSamplers(2, 1, &direct3d::states().bilinear_clamp_sampler.ptr());
        direct3d::api().devcon4->VSSetSamplers(3, 1, &direct3d::states().comparison_linear_clamp_sampler.ptr());

        direct3d::api().devcon4->CSSetSamplers(0, 1, &direct3d::states().bilinear_wrap_sampler.ptr());
        direct3d::api().devcon4->CSSetSamplers(1, 1, &direct3d::states().anisotropic_wrap_sampler.ptr());
        direct3d::api().devcon4->CSSetSamplers(2, 1, &direct3d::states().bilinear_clamp_sampler.ptr());
        direct3d::api().devcon4->CSSetSamplers(3, 1, &direct3d::states().comparison_linear_clamp_sampler.ptr());

        scene_->FrameBegin();

        api().devcon4->RSSetViewports(1, &viewport_);
        std::vector <ID3D11RenderTargetView *> gbuffer_target_views = {
                                     gbuffer_.albedo->render_target_view(),
                                     gbuffer_.normals->render_target_view(),
                                     gbuffer_.roughness_metalness_transmittance_ao->render_target_view(),
                                     gbuffer_.emission->render_target_view(),
                                     gbuffer_.entity_id->render_target_view()
        };
        api().devcon4->OMSetRenderTargets(5, gbuffer_target_views.data(), depth_stencil_.depth_stencil_view());
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth_write_stencil_replace, 0);
        scene_->DeferredRender(gbuffer_, depth_stencil_.depth_stencil_view());

        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth_write_stencil_read, 0);
        deferred_resolve_->Process(gbuffer_, scene_.get(), depth_stencil_.depth_stencil_view());

        api().devcon4->RSSetViewports(1, &viewport_);
        api().devcon4->OMSetRenderTargets(1, &hdr_target_.render_target_view(), depth_stencil_.depth_stencil_view());

        scene_->ForwardRender(per_frame_, gbuffer_, depth_stencil_.depth_stencil_view());

        hdr_to_ldr_layer_->OnProcess(hdr_target_);

        // Render imgui

        imgui_layer_->Begin();
        OnGuiRender();
        imgui_layer_->End();
        // End frame
        FrameEnd();
    }
    void DeferredHDRRenderPipeline::OnUpdate()
    {
        core::RenderPipeline::OnUpdate();
        scene_->Update();
    }
    void DeferredHDRRenderPipeline::OnTick(float dt)
    {
        core::RenderPipeline::OnTick(dt);
        scene_->Tick(dt);
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
        const core::math::vec4 empty_vec{ 0.0f, 0.0f, 0.0f, 1.0f };
        api().devcon4->ClearRenderTargetView(hdr_target_.render_target_view(), empty_vec.data.data());
        api().devcon4->ClearRenderTargetView(gbuffer_.normals->render_target_view(), empty_vec.data.data());
        api().devcon4->ClearRenderTargetView(gbuffer_.albedo->render_target_view(), empty_vec.data.data());
        api().devcon4->ClearRenderTargetView(gbuffer_.roughness_metalness_transmittance_ao->render_target_view(), empty_vec.data.data());
        api().devcon4->ClearRenderTargetView(gbuffer_.emission->render_target_view(), empty_vec.data.data());
        api().devcon4->ClearRenderTargetView(gbuffer_.entity_id->render_target_view(), empty_vec.data.data());
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
        per_frame_.time_now = core::Engine::TimeFromStart();
        per_frame_.time_since_last_frame = timer.elapsed();
        timer.reset();

        per_frame_buffer_.Bind(ShaderType::VertexShader, 0);
        per_frame_buffer_.Bind(ShaderType::HullShader, 0);
        per_frame_buffer_.Bind(ShaderType::DomainShader, 0);
        per_frame_buffer_.Bind(ShaderType::GeometryShader, 0);
        per_frame_buffer_.Bind(ShaderType::PixelShader, 0);
        per_frame_buffer_.Bind(ShaderType::ComputeShader, 0);
        per_frame_buffer_.Update(per_frame_);
    }
    void DeferredHDRRenderPipeline::PostProcess()
    {
    }
    void DeferredHDRRenderPipeline::FrameEnd()
    {
        swapchain_present_->OnFrameEnd(static_cast<direct3d::RenderTargetBase &>(*output_target_));
    }
}