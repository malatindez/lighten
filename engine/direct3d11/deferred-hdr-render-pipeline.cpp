#include "deferred-hdr-render-pipeline.hpp"
#include "components/components.hpp"
#include "subsystems/core/input-layer.hpp"
#include "subsystems/render/skybox-manager.hpp"
#include "core/engine.hpp"

#include "imgui_internal.h"  
namespace lighten::direct3d
{
    DeferredHDRRenderPipeline::DeferredHDRRenderPipeline(std::shared_ptr<core::Window> window, std::shared_ptr<SwapchainRenderTarget> const &output_target)
        : core::RenderPipeline(),
          hdr_target_{DXGI_FORMAT_R16G16B16A16_FLOAT},
          output_target_{output_target},
        output_render_texture_{ DXGI_FORMAT_R8G8B8A8_UNORM }
    {
        viewport_.MinDepth = 0.0f;
        viewport_.MaxDepth = 1.0f;
        viewport_.TopLeftX = 0;
        viewport_.TopLeftY = 0;
        hdr_target_.init();
        output_render_texture_.init();
        InitImGuiLayer(window);
        hdr_to_ldr_layer_ = std::make_shared<render::HDRtoLDRLayer>(*output_target_);
        window_ = window;

        deferred_resolve_ = std::make_shared<render::DeferredResolve>(hdr_target_);
        hdr_to_ldr_layer_ = std::make_shared<render::HDRtoLDRLayer>(*output_target_);
        swapchain_present_ = std::make_shared<render::PresentSwapchain>();

        gbuffer_.normals = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_SNORM);
        gbuffer_.albedo = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_FLOAT);
        gbuffer_.roughness_metalness_transmittance_ao = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_SNORM);
        gbuffer_.sheen = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_SNORM);
        gbuffer_.emission = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_FLOAT);
        gbuffer_.entity_id = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R32_UINT);
        imgui_layer_->BlockEvents(false);
    }
    void DeferredHDRRenderPipeline::WindowSizeChanged(glm::ivec2 const &input_size)
    {
        if (framebuffer_size_.x <= 1 || framebuffer_size_.y <= 1)
        {
            framebuffer_size_ = input_size;
        }
        auto const& size = framebuffer_size_;

        // TODO: rewrite system to correctly support framebuffer resizing
        hdr_target_.SizeResources(size);
        output_render_texture_.SizeResources(size);
        output_target_->SizeResources(size);
        gbuffer_.normals->SizeResources(size);
        gbuffer_.albedo->SizeResources(size);
        gbuffer_.roughness_metalness_transmittance_ao->SizeResources(size);
        gbuffer_.sheen->SizeResources(size);
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
        desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.MiscFlags = 0;
        direct3d::AlwaysAssert(direct3d::api().device->CreateTexture2D(&desc, nullptr, &depth_texture_copy_.reset()), "Failed to create depth texture copy");
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;
        srv_desc.Texture2D.MostDetailedMip = 0;
        direct3d::AlwaysAssert(direct3d::api().device->CreateShaderResourceView(depth_texture_copy_.ptr(), &srv_desc, &depth_texture_copy_srv_.reset()), "Failed to create depth texture copy SRV");
        desc = gbuffer_.normals->render_target_description();
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.MiscFlags = 0;
        direct3d::AlwaysAssert(direct3d::api().device->CreateTexture2D(&desc, nullptr, &normals_texture_copy_.reset()), "Failed to create normal texture copy");
        srv_desc.Format = desc.Format;
        direct3d::AlwaysAssert(direct3d::api().device->CreateShaderResourceView(normals_texture_copy_.ptr(), &srv_desc, &normals_texture_copy_srv_.reset()), "Failed to create normal texture copy SRV");

        // Set up the viewport.
        viewport_.Width = (float)size.x;
        viewport_.Height = (float)size.y;

        direct3d::api().devcon4->RSSetViewports(1, &viewport_);
    }
    void DeferredHDRRenderPipeline::OnRender()
    {
        if (framebuffer_size_ != hdr_target_.size())
        {
            WindowSizeChanged(framebuffer_size_);
        }

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
        std::vector<ID3D11RenderTargetView*> gbuffer_target_views = {
            gbuffer_.albedo->render_target_view(),
            gbuffer_.normals->render_target_view(),
            gbuffer_.roughness_metalness_transmittance_ao->render_target_view(),
            gbuffer_.sheen->render_target_view(),
            gbuffer_.emission->render_target_view(),
            gbuffer_.entity_id->render_target_view() };
        api().devcon4->OMSetRenderTargets(5, gbuffer_target_views.data(), depth_stencil_.depth_stencil_view());
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth_write_stencil_replace, 1);
        scene_->DeferredRender(gbuffer_);

        direct3d::api().devcon4->CopyResource(depth_texture_copy_, depth_stencil_.depth_buffer());
        direct3d::api().devcon4->CopyResource(normals_texture_copy_, gbuffer_.normals->render_target());

        scene_->DeferredRender(gbuffer_, depth_stencil_.depth_stencil_view(), depth_texture_copy_srv_, normals_texture_copy_srv_);

        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().no_depth_stencil_read, 1);
        deferred_resolve_->Process(gbuffer_, scene_.get(), depth_stencil_.depth_stencil_view(), depth_texture_copy_srv_);

        api().devcon4->RSSetViewports(1, &viewport_);
        api().devcon4->OMSetRenderTargets(1, &hdr_target_.render_target_view(), depth_stencil_.depth_stencil_view());

        scene_->ForwardRender(per_frame_, gbuffer_, depth_stencil_.depth_stencil_view(), depth_texture_copy_srv_, normals_texture_copy_srv_);

        hdr_to_ldr_layer_->OnProcess(hdr_target_, output_render_texture_);

        ID3D11RenderTargetView* rtv = output_target_->render_target_view();
        api().devcon4->OMSetRenderTargets(1, &output_target_->render_target_view(), nullptr);

        constexpr glm::vec4 kCleanColor{ 0.0f, 0.0f, 0.0f, 1.0f };
        direct3d::api().devcon4->OMSetRenderTargets(1, &rtv, direct3d::null_dsv);
        direct3d::api().devcon4->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&kCleanColor));


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
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        ImGui::End();

        ImGui::Begin("Rendered Scene");
        auto framebuffer_size = ImGui::GetContentRegionAvail();

        ID3D11ShaderResourceView* renderTextureSRV = output_render_texture_.shader_resource_view();
        ImGui::Image((void*)renderTextureSRV, ImVec2(output_render_texture_.size().x, output_render_texture_.size().y));

        framebuffer_pos_ = glm::ivec2{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
        framebuffer_size_ = glm::ivec2{ framebuffer_size.x, framebuffer_size.y };
        framebuffer_size_ = glm::max(framebuffer_size_, glm::ivec2{16});

        framebuffer_pos_ += glm::ivec2{ ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowContentRegionMin().y };

        glm::vec2 framebuffer_end = glm::vec2{ framebuffer_pos_.x, framebuffer_pos_.y } + glm::vec2{ framebuffer_size_.x, framebuffer_size_.y };

        ImVec2 mousePos = ImGui::GetMousePos();
 
        glm::ivec2 relative_mouse_pos{ -1 };
        // Check if the mouse is within the framebuffer area
        if (mousePos.x >= framebuffer_pos_.x && mousePos.x <= (framebuffer_pos_.x + framebuffer_size_.x) &&
            mousePos.y >= framebuffer_pos_.y && mousePos.y <= (framebuffer_pos_.y + framebuffer_size_.y))
        {
            relative_mouse_pos = glm::ivec2(mousePos.x - framebuffer_pos_.x, mousePos.y - framebuffer_pos_.y);
        }

        bool isWindowFocused = ImGui::IsWindowFocused();
        bool isWindowHovered = ImGui::IsWindowHovered();
        // Handling keyboard when the window is focused
        if (isWindowFocused)
        {
            if (show_focus_rect)
            {
                ImGui::GetForegroundDrawList()->AddRect(ImVec2(framebuffer_pos_.x, framebuffer_pos_.y), ImVec2(framebuffer_end.x, framebuffer_end.y), IM_COL32(255, 255, 0, 255));
            }
            for (int key = 0; key < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); ++key)
            {
                if (ImGui::IsKeyPressed(key))
                {
                    // Emit KeyPressedEvent
                    lighten::core::events::KeyPressedEvent event(key);
                    window_->GetEventCallback()(event);
                }
            }
        }

        for (int key = 0; key < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); ++key)
        {
            if (ImGui::IsKeyReleased(key))
            {
                // Emit KeyReleasedEvent
                lighten::core::events::KeyReleasedEvent event(key);
                window_->GetEventCallback()(event);
            }
        }
        // Handling mouse clicks when the window is hovered
        if ((isWindowHovered || isWindowFocused) && (relative_mouse_pos != glm::ivec2{ -1 }))
        {
            lighten::core::events::MouseMovedEvent mouseMovedEvent(relative_mouse_pos);
            window_->GetEventCallback()(mouseMovedEvent);
            for (int button = 0; button < ImGuiMouseButton_COUNT; ++button)
            {
                if (ImGui::IsMouseClicked(button))
                {
                    // Emit MouseButtonPressedEvent

                    lighten::core::events::MouseButtonPressedEvent event(button, relative_mouse_pos);
                    window_->GetEventCallback()(event);
                }

                if (ImGui::IsMouseReleased(button))
                {
                    // Emit MouseButtonReleasedEvent

                    lighten::core::events::MouseButtonReleasedEvent event(button, relative_mouse_pos);
                    window_->GetEventCallback()(event);
                }
            }
        }

        if (ImGui::GetIO().MouseWheel != 0)
        {
            lighten::core::events::MouseScrollEvent event(static_cast<int16_t>(ImGui::GetIO().MouseWheel * 100), relative_mouse_pos);
            window_->GetEventCallback()(event);
        }
        ImGui::End();

        ImGui::Begin("Viewport settings");
        ImGui::Checkbox("Show focus rect", &show_focus_rect);
        ImGui::End();
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
        const glm::vec4 empty_vec{0.0f, 0.0f, 0.0f, 1.0f};
        api().devcon4->ClearRenderTargetView(hdr_target_.render_target_view(), reinterpret_cast<const float *>(&empty_vec));
        api().devcon4->ClearRenderTargetView(gbuffer_.normals->render_target_view(), reinterpret_cast<const float *>(&empty_vec));
        api().devcon4->ClearRenderTargetView(gbuffer_.albedo->render_target_view(), reinterpret_cast<const float *>(&empty_vec));
        api().devcon4->ClearRenderTargetView(gbuffer_.roughness_metalness_transmittance_ao->render_target_view(), reinterpret_cast<const float *>(&empty_vec));
        api().devcon4->ClearRenderTargetView(gbuffer_.sheen->render_target_view(), reinterpret_cast<const float *>(&empty_vec));
        api().devcon4->ClearRenderTargetView(gbuffer_.emission->render_target_view(), reinterpret_cast<const float *>(&empty_vec));
        api().devcon4->ClearRenderTargetView(gbuffer_.entity_id->render_target_view(), reinterpret_cast<const float *>(&empty_vec));
        api().devcon4->ClearDepthStencilView(depth_stencil_.depth_stencil_view(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

        auto const &camera = scene_->main_camera->get_last_camera_state();
        per_frame_.view = camera.view;
        per_frame_.projection = camera.projection;
        per_frame_.view_projection = camera.view_projection;
        per_frame_.inv_view = camera.inv_view;
        per_frame_.inv_projection = camera.inv_projection;
        per_frame_.inv_view_projection = camera.inv_view_projection;
        per_frame_.screen_resolution = glm::vec2{viewport_.Width, viewport_.Height};
        per_frame_.mouse_position = glm::vec2{core::InputLayer::instance()->mouse_position()};
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
        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        swapchain_present_->OnFrameEnd(static_cast<direct3d::RenderTargetBase &>(*output_target_));

    }
} // namespace lighten::direct3d