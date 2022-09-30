#include "windows-render-pipeline.hpp"
#include "core/engine.hpp"
using namespace engine::direct3d;
namespace engine::platform::windows
{
    RenderPipeline::RenderPipeline() : core::RenderPipeline()
    {
        window_ = std::make_shared<Window>();
        initialize_d3d();
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
    void RenderPipeline::OnEvent(core::events::Event &e)
    {
        core::RenderPipeline::OnEvent(e);
        if (e.type() == core::events::EventType::WindowResize)
        {
            windowSizeChanged();
        }
    }

    void RenderPipeline::FrameBegin()
    {
        ID3D11RenderTargetView *view = frame_buffer_view_;
        api().devcon4->OMSetRenderTargets(1, &view, depth_buffer_view_);

        api().devcon4->ClearRenderTargetView(frame_buffer_view_, sky_color_.data.data());
        api().devcon4->ClearDepthStencilView(depth_buffer_view_, D3D11_CLEAR_DEPTH, 0.0f, 0);

        per_frame_buffer_.Update(per_frame_);
        per_frame_buffer_.Bind(ShaderType::VertexShader, 0);
    }
    void RenderPipeline::FrameEnd()
    {
        // switch the back buffer and the front buffer
        swapchain_->Present(1, 0);
    }

    namespace
    {
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

    void RenderPipeline::initializeFramebuffer()
    {
        ID3D11Texture2D *frame_buffer = nullptr;

        AlwaysAssert(swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&frame_buffer)),
                     "Failed to get frame buffer");

        ID3D11RenderTargetView *frame_buffer_view = nullptr;

        AlwaysAssert(api().device->CreateRenderTargetView(frame_buffer, nullptr, &frame_buffer_view),
                     "Failed to initialize framebuffer");
        frame_buffer_ = frame_buffer;
        frame_buffer_view_ = frame_buffer_view;
    }

    void RenderPipeline::initializeDepthbuffer()
    {
        frame_buffer_->GetDesc(&depth_buffer_desc_); // base on frame_buffer properties

        depth_buffer_desc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_buffer_desc_.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        ID3D11Texture2D *depth_buffer;

        AlwaysAssert(api().device->CreateTexture2D(&depth_buffer_desc_, nullptr, &depth_buffer),
                     "Failed to initialize depthbuffer");

        ID3D11DepthStencilView *depth_buffer_view;

        AlwaysAssert(api().device->CreateDepthStencilView(depth_buffer, nullptr, &depth_buffer_view),
                     "Failed to initialize depthbuffer");

        depth_buffer_ = depth_buffer;
        depth_buffer_view_ = depth_buffer_view;
    }

    void RenderPipeline::initialize_d3d()
    {
        swapchain_ = initializeSwapchain(window()->handle(), window()->size());
        windowSizeChanged();
    }
}