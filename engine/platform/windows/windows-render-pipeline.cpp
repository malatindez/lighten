#include "windows-render-pipeline.hpp"
#include "direct3d11/direct3d11.hpp"
using namespace engine::direct3d;
namespace engine::platform::windows
{
    RenderPipeline::RenderPipeline() : core::RenderPipeline()
    {
        window_ = std::make_shared<Window>();
        D3D11_RASTERIZER_DESC1 rasterizer_desc = {};
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_BACK;

        ID3D11RasterizerState1 *rasterizer_state;

        device5->CreateRasterizerState1(&rasterizer_desc,
                                        &rasterizer_state);

        D3D11_SAMPLER_DESC sampler_desc = {};
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        ID3D11SamplerState *sampler_state;

        device5->CreateSamplerState(&sampler_desc, &sampler_state);

        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
        depth_stencil_desc.DepthEnable = TRUE;
        depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depth_stencil_desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

        ID3D11DepthStencilState *depth_stencil_state;

        device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);

        rasterizer_state_ = rasterizer_state;
        sampler_state_ = sampler_state;
        depth_stencil_state_ = depth_stencil_state;

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
        devcon4->OMSetRenderTargets(1, &view, nullptr);

        devcon4->RSSetState(rasterizer_state_);
        ID3D11SamplerState *sampler_state = sampler_state_;
        devcon4->PSSetSamplers(0, 1, &sampler_state);
        ID3D11RenderTargetView *frame_buffer_view = window_->frame_buffer_view();

        devcon4->OMSetRenderTargets(1, &frame_buffer_view, window_->depth_buffer_view());
        devcon4->OMSetDepthStencilState(depth_stencil_state_, 0);
        devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

        // clear the back buffer to a deep blue
        devcon4->ClearRenderTargetView(window_->frame_buffer_view(), sky_color_.data.data());
        devcon4->ClearDepthStencilView(window_->depth_buffer_view(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
    void RenderPipeline::FrameEnd()
    {

        // switch the back buffer and the front buffer
        window_->swapchain()->Present(1, 0);
    }

}