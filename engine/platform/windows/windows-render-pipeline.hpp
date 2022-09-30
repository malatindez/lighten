#pragma once
#include "direct3d11/direct3d11.hpp"
#include "core/layers/render-pipeline.hpp"
#include "windows-window.hpp"
#include "render/common.hpp"
namespace engine::platform::windows
{
    class RenderPipeline : public core::RenderPipeline
    {
    public:
        RenderPipeline();
        [[nodiscard]] inline core::math::vec4 &sky_color() noexcept { return sky_color_; }
        [[nodiscard]] inline core::math::vec4 const &sky_color() const noexcept { return sky_color_; }
        [[nodiscard]] inline std::shared_ptr<Window> window() const noexcept { return window_; }
        [[nodiscard]] inline std::shared_ptr<core::Window> window_base() const noexcept { return window_; }
        [[nodiscard]] inline render::PerFrame const &per_frame() const noexcept { return per_frame_; }
        [[nodiscard]] inline render::PerFrame &per_frame() noexcept { return per_frame_; }
        [[nodiscard]] inline IDXGISwapChain1 *swapchain() noexcept { return swapchain_.ptr(); }
        [[nodiscard]] inline ID3D11Texture2D *frame_buffer() noexcept { return frame_buffer_.ptr(); }
        [[nodiscard]] inline ID3D11RenderTargetView *frame_buffer_view() noexcept { return frame_buffer_view_.ptr(); }
        [[nodiscard]] inline ID3D11Texture2D *depth_buffer() noexcept { return depth_buffer_.ptr(); }
        [[nodiscard]] inline ID3D11DepthStencilView *depth_buffer_view() noexcept { return depth_buffer_view_.ptr(); }
        [[nodiscard]] inline D3D11_TEXTURE2D_DESC depth_buffer_desc() const noexcept { return depth_buffer_desc_; }

        void OnRender() override;
        void OnUpdate() override;
        void OnEvent(core::events::Event &) override;
    private:
        void initialize_d3d();
        void initializeFramebuffer();
        void initializeDepthbuffer();
        void windowSizeChanged()
        {
            if (frame_buffer_.valid())
            {
                direct3d::api().devcon4->OMSetRenderTargets(0, nullptr, nullptr);
                frame_buffer_.reset();
                frame_buffer_view_.reset();
                depth_buffer_.reset();
                depth_buffer_view_.reset();
                swapchain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            }
            initializeFramebuffer();
            initializeDepthbuffer();

            // Set up the viewport.
            D3D11_VIEWPORT vp;
            vp.Width = (float)window()->size().x;
            vp.Height = (float)window()->size().y;
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            vp.TopLeftX = 0;
            vp.TopLeftY = 0;
            direct3d::api().devcon4->RSSetViewports(1, &vp);
        }
        void FrameBegin() override;
        void FrameEnd() override;
        core::math::vec4 sky_color_{ core::math::vec3{0.25f}, 0.0f };
        std::shared_ptr<Window> window_;
        render::PerFrame per_frame_;
        direct3d::DynamicUniformBuffer<engine::render::PerFrame> per_frame_buffer_{};

        direct3d::SwapChain1 swapchain_;
        direct3d::Texture2D frame_buffer_;
        direct3d::RenderTargetView frame_buffer_view_;
        direct3d::Texture2D depth_buffer_;
        direct3d::DepthStencilView depth_buffer_view_;
        D3D11_TEXTURE2D_DESC depth_buffer_desc_;
    };
}