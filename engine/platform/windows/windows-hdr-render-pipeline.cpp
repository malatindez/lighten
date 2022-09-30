#include "windows-hdr-render-pipeline.hpp"
#include "core/engine.hpp"
using namespace engine::direct3d;
namespace engine::platform::windows
{
    HDRRenderPipeline::HDRRenderPipeline()
        : core::RenderPipeline(),
        hdr_target_{ DXGI_FORMAT_R16G16B16A16_FLOAT },
        ldr_target_{ DXGI_FORMAT_UNKNOWN }
    {
        window_ = std::make_shared<Window>();
        hdr_target_.init();
        initialize_d3d();
        InitImGuiLayer(std::static_pointer_cast<core::Window>(window_));
        post_processing_ = std::make_shared<render::PostProcessing>();
        hdr_to_ldr_layer_ = std::make_shared<render::HDRtoLDRLayer>(ldr_target_);
        post_processing_->AddLayer(hdr_to_ldr_layer_);
        this->PushLayer(post_processing_);
    }

    void HDRRenderPipeline::OnRender()
    {
        if (!window_->alive()) [[unlikely]]
            return;
        FrameBegin();
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
        window_->OnUpdate();
        core::RenderPipeline::OnUpdate();
    }
    void HDRRenderPipeline::OnEvent(core::events::Event &e)
    {
        if (e.type() == core::events::EventType::WindowResize)
        {
            windowSizeChanged();
        }
        core::RenderPipeline::OnEvent(e);
    }

    void HDRRenderPipeline::FrameBegin()
    {
        api().devcon4->OMSetRenderTargets(1, &hdr_target_.render_target_view(), depth_stencil_.depth_stencil_view());

        api().devcon4->ClearRenderTargetView(hdr_target_.render_target_view(), sky_color_.data.data());
        api().devcon4->ClearDepthStencilView(depth_stencil_.depth_stencil_view(), D3D11_CLEAR_DEPTH, 0.0f, 0);

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
        post_processing_->OnFrameEnd();
        // switch the back buffer and the front buffer
        swapchain_->Present(1, 0);
    }

    namespace {
        SwapChain1 initializeSwapchain(HWND hWnd, core::math::ivec2 const &window_size)
        {
            DXGI_SWAP_CHAIN_DESC1 desc;
            ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC1));
            desc.Width = window_size.x;
            desc.Height = window_size.y;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.Stereo = FALSE;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.BufferCount = 2;
            desc.Scaling = DXGI_SCALING_NONE;
            desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            desc.Flags = 0;

            IDXGISwapChain1 *swapchain = nullptr;
            SetLastError(0);
            AlwaysAssert(api().factory5->CreateSwapChainForHwnd(api().device, hWnd, &desc, nullptr, nullptr, &swapchain),
                         "Failed to create the swapchain");
            return SwapChain1{ swapchain };
        }
    }

    void HDRRenderPipeline::initialize_d3d()
    {
        swapchain_ = initializeSwapchain(window()->handle(), window()->size());
        ldr_target_.init(swapchain_);
        windowSizeChanged();
    }
}