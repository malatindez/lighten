#pragma once
#include "direct3d11/direct3d11.hpp"
#include "core/layers/render-pipeline.hpp"
#include "windows-window.hpp"
#include "render/common.hpp"
#include "subsystems/render/post-processing.hpp"
#include "render/hdr-to-ldr-layer.hpp"
namespace engine::platform::windows
{
    class HDRRenderPipeline : public core::RenderPipeline
    {
    public:
        HDRRenderPipeline();
        [[nodiscard]] inline core::math::vec4 &sky_color() noexcept { return sky_color_; }
        [[nodiscard]] inline core::math::vec4 const &sky_color() const noexcept { return sky_color_; }
        [[nodiscard]] inline std::shared_ptr<Window> window() const noexcept { return window_; }
        [[nodiscard]] inline std::shared_ptr<core::Window> window_base() const noexcept { return window_; }
        [[nodiscard]] inline render::PerFrame const &per_frame() const noexcept { return per_frame_; }
        [[nodiscard]] inline render::PerFrame &per_frame() noexcept { return per_frame_; }
        [[nodiscard]] inline IDXGISwapChain1 *swapchain() noexcept { return swapchain_.ptr(); }
        [[nodiscard]] inline std::shared_ptr<render::HDRtoLDRLayer> hdr_to_ldr_layer() const noexcept { return hdr_to_ldr_layer_; }
        void OnRender() override;
        void OnUpdate() override;
        void OnEvent(core::events::Event &) override;
    private:
        void initialize_d3d();
        void windowSizeChanged()
        {
            hdr_target_.SizeResources(window()->size());
            ldr_target_.SizeResources(window()->size());
            D3D11_TEXTURE2D_DESC desc = ldr_target_.render_target_description();
            desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            desc.Width = static_cast<uint32_t>(window()->size().x);
            desc.Height = static_cast<uint32_t>(window()->size().y);
            depth_stencil_.init(&desc, nullptr);

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
        void PostProcess();
        void FrameEnd() override;
        core::math::vec4 sky_color_{ core::math::vec3{0.25f}, 0.0f };
        std::shared_ptr<Window> window_;
        render::PerFrame per_frame_;
        direct3d::DynamicUniformBuffer<engine::render::PerFrame> per_frame_buffer_{};
        direct3d::SwapChain1 swapchain_;
        direct3d::DepthStencil depth_stencil_;
        direct3d::RenderTarget hdr_target_;
        direct3d::SwapchainRenderTarget ldr_target_;
        std::shared_ptr<render::HDRtoLDRLayer> hdr_to_ldr_layer_;
        std::shared_ptr<render::PostProcessing> post_processing_;
    };
}