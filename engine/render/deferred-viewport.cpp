#include "deferred-viewport.hpp"
namespace lighten::render
{
    DeferredViewport::DeferredViewport(std::string viewport_name, glm::ivec2 const& initial_size, std::shared_ptr<core::World> world) :
            ViewportBase{ viewport_name, ViewportType::Deferred },
            viewport_size_{ initial_size },
            camera_controller_{ &world->registry(), entt::null, viewport_size_ },
            hdr_target_{DXGI_FORMAT_R16G16B16A16_FLOAT},
            output_render_texture_{ DXGI_FORMAT_R8G8B8A8_UNORM }    
    {
        hdr_target_.init();
        output_render_texture_.init();
        hdr_to_ldr_layer_ = std::make_shared<render::HDRtoLDRLayer>(*output_target_);

        deferred_resolve_ = std::make_shared<render::DeferredResolve>(hdr_target_);
        swapchain_present_ = std::make_shared<render::PresentSwapchain>();

        gbuffer_.normals = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_SNORM);
        gbuffer_.albedo = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_FLOAT);
        gbuffer_.roughness_metalness_transmittance_ao = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_SNORM);
        gbuffer_.sheen = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_SNORM);
        gbuffer_.emission = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R16G16B16A16_FLOAT);
        gbuffer_.entity_id = std::make_shared<direct3d::RenderTarget>(DXGI_FORMAT_R32_UINT);
    }
    void DeferredViewport::OnResize(glm::ivec2 const& new_size)
    {
        if (framebuffer_size_.x <= 1 || framebuffer_size_.y <= 1)
        {
            framebuffer_size_ = glm::max(glm::ivec2{new_size}, glm::ivec2{32, 32});
        }

        auto const& size = framebuffer_size_;

        // TODO: rewrite system to correctly support framebuffer resizing
        hdr_target_.SizeResources(size);
        output_render_texture_.SizeResources(size);

        gbuffer_.normals->SizeResources(size);
        gbuffer_.albedo->SizeResources(size);
        gbuffer_.roughness_metalness_transmittance_ao->SizeResources(size);
        gbuffer_.sheen->SizeResources(size);
        gbuffer_.emission->SizeResources(size);
        gbuffer_.entity_id->SizeResources(size);

        D3D11_TEXTURE2D_DESC desc = hdr_target_.render_target_description();
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
        direct3d::AlwaysAssert(
            direct3d::api().device->CreateTexture2D(&desc, nullptr, &depth_texture_copy_.reset()), 
            "Failed to create depth texture copy");

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;
        srv_desc.Texture2D.MostDetailedMip = 0;
        direct3d::AlwaysAssert(
            direct3d::api().device->CreateShaderResourceView(depth_texture_copy_.ptr(), 
                                                             &srv_desc,
                                                             &depth_texture_copy_srv_.reset()), 
            "Failed to create depth texture copy SRV");

        desc = gbuffer_.normals->render_target_description();
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.MiscFlags = 0;
        direct3d::AlwaysAssert(
            direct3d::api().device->CreateTexture2D(&desc, 
                                                    nullptr, 
                                                    &normals_texture_copy_.reset()), 
            "Failed to create normal texture copy");

        srv_desc.Format = desc.Format;
        direct3d::AlwaysAssert(
            direct3d::api().device->CreateShaderResourceView(normals_texture_copy_.ptr(), 
                                                             &srv_desc, 
                                                             &normals_texture_copy_srv_.reset()), 
            "Failed to create normal texture copy SRV");
    }
       
    void DeferredViewport::OnRender()
    {
        using namespace direct3d;
        if (framebuffer_size_ != hdr_target_.size())
        {
            OnResize(framebuffer_size_);
        }

        FrameBegin(); 
        
        //scene_->FrameBegin();

        std::vector<ID3D11RenderTargetView*> gbuffer_target_views = {
            gbuffer_.albedo->render_target_view(),
            gbuffer_.normals->render_target_view(),
            gbuffer_.roughness_metalness_transmittance_ao->render_target_view(),
            gbuffer_.sheen->render_target_view(),
            gbuffer_.emission->render_target_view(),
            gbuffer_.entity_id->render_target_view() };
        api().devcon4->OMSetRenderTargets(5, gbuffer_target_views.data(), depth_stencil_.depth_stencil_view());
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth_write_stencil_replace, 1);
        
        //scene_->DeferredRender(gbuffer_);

        direct3d::api().devcon4->CopyResource(depth_texture_copy_, depth_stencil_.depth_buffer());
        direct3d::api().devcon4->CopyResource(normals_texture_copy_, gbuffer_.normals->render_target());

        //scene_->DeferredRender(gbuffer_, depth_stencil_.depth_stencil_view(), depth_texture_copy_srv_, normals_texture_copy_srv_);

        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().no_depth_stencil_read, 1);
        //deferred_resolve_->Process(gbuffer_, scene_.get(), depth_stencil_.depth_stencil_view(), depth_texture_copy_srv_);


        api().devcon4->OMSetRenderTargets(1, &hdr_target_.render_target_view(), depth_stencil_.depth_stencil_view());

        //scene_->ForwardRender(per_frame_, gbuffer_, depth_stencil_.depth_stencil_view(), depth_texture_copy_srv_, normals_texture_copy_srv_);

        hdr_to_ldr_layer_->OnProcess(hdr_target_, output_render_texture_);




        // End frame
        FrameEnd();
    }
    void DeferredViewport::OnUpdate()
    {
        core::LayerStack::OnUpdate();
    }
    void DeferredViewport::OnTick(float dt) 
    {
        core::LayerStack::OnTick(dt);
    }
    void DeferredViewport::OnEvent(core::events::Event& event)
    {
        core::LayerStack::OnEvent(event);
    }
    

    void DeferredViewport::FrameBegin()
    {
        using namespace lighten::direct3d;
        const glm::vec4 kCleanColor{0.0f, 0.0f, 0.0f, 1.0f};

        api().devcon4->ClearRenderTargetView(hdr_target_.render_target_view(), reinterpret_cast<const float *>(&kCleanColor));
        api().devcon4->ClearRenderTargetView(gbuffer_.normals->render_target_view(), reinterpret_cast<const float *>(&kCleanColor));
        api().devcon4->ClearRenderTargetView(gbuffer_.albedo->render_target_view(), reinterpret_cast<const float *>(&kCleanColor));
        api().devcon4->ClearRenderTargetView(gbuffer_.roughness_metalness_transmittance_ao->render_target_view(), reinterpret_cast<const float *>(&kCleanColor));
        api().devcon4->ClearRenderTargetView(gbuffer_.sheen->render_target_view(), reinterpret_cast<const float *>(&kCleanColor));
        api().devcon4->ClearRenderTargetView(gbuffer_.emission->render_target_view(), reinterpret_cast<const float *>(&kCleanColor));
        api().devcon4->ClearRenderTargetView(gbuffer_.entity_id->render_target_view(), reinterpret_cast<const float *>(&kCleanColor));
        api().devcon4->ClearDepthStencilView(depth_stencil_.depth_stencil_view(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);



        auto const &camera = camera_controller_.get_last_camera_state();
        per_frame_.view = camera.view;
        per_frame_.projection = camera.projection;
        per_frame_.view_projection = camera.view_projection;
        per_frame_.inv_view = camera.inv_view;
        per_frame_.inv_projection = camera.inv_projection;
        per_frame_.inv_view_projection = camera.inv_view_projection;
        per_frame_.screen_resolution = framebuffer_size_;
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
    void DeferredViewport::PostProcess()
    {
    }
    void DeferredViewport::FrameEnd()
    {
    }
}