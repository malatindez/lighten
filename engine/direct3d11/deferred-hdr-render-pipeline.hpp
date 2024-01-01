#pragma once
#include "direct3d11/direct3d11.hpp"
#include "core/layers/render-pipeline.hpp"
#include "render/common.hpp"
#include "subsystems/render/post-processing.hpp"
#include "render/hdr-to-ldr-layer.hpp"
#include "render/deferred-resolve.hpp"
#include "render/present-swapchain.hpp"
#include "render/renderer.hpp"
namespace engine::direct3d
{
    class DeferredHDRRenderPipeline : public core::RenderPipeline
    {
    public:
        DeferredHDRRenderPipeline(std::shared_ptr<core::Window> window, std::shared_ptr<SwapchainRenderTarget> const &output_target);
        DeferredHDRRenderPipeline(DeferredHDRRenderPipeline const &) = delete;
        DeferredHDRRenderPipeline(DeferredHDRRenderPipeline &&) = delete;
        DeferredHDRRenderPipeline &operator=(DeferredHDRRenderPipeline const &) = delete;
        DeferredHDRRenderPipeline &operator=(DeferredHDRRenderPipeline &&) = delete;

        [[nodiscard]] inline glm::vec4 &sky_color() noexcept { return sky_color_; }
        [[nodiscard]] inline glm::vec4 const &sky_color() const noexcept { return sky_color_; }
        [[nodiscard]] inline render::PerFrame const &per_frame() const noexcept { return per_frame_; }
        [[nodiscard]] inline render::PerFrame &per_frame() noexcept { return per_frame_; }
        [[nodiscard]] inline std::shared_ptr<render::HDRtoLDRLayer> hdr_to_ldr_layer() const noexcept { return hdr_to_ldr_layer_; }
        [[nodiscard]] inline std::shared_ptr<core::Window> window() noexcept { return window_; }
        [[nodiscard]] inline auto deferred_resolve() noexcept { return deferred_resolve_; }
        [[nodiscard]] inline auto swapchain_present() noexcept { return swapchain_present_; }
        void OnRender() override;
        void OnUpdate() override;
        void OnTick(float) override;
        void OnEvent(core::events::Event &) override;
        void WindowSizeChanged(glm::ivec2 const &size);

    public:
        void FrameBegin() override;
        void FrameEnd() override;
        void PostProcess();
        mal_toolkit::HighResolutionTimer timer;
        glm::vec4 sky_color_{glm::vec3{0.25f}, 0.0f};
        render::PerFrame per_frame_;
        DynamicUniformBuffer<engine::render::PerFrame> per_frame_buffer_{};
        DepthStencil depth_stencil_;
        direct3d::RenderTarget hdr_target_;
        std::shared_ptr<direct3d::SwapchainRenderTarget> output_target_;
        D3D11_VIEWPORT viewport_;

        render::GBuffer gbuffer_;

        // TODO:
        // move those to render graph or something
        std::shared_ptr<render::DeferredResolve> deferred_resolve_ = nullptr;
        std::shared_ptr<render::HDRtoLDRLayer> hdr_to_ldr_layer_ = nullptr;
        std::shared_ptr<render::PresentSwapchain> swapchain_present_ = nullptr;
        direct3d::Texture2D depth_texture_copy_;
        direct3d::ShaderResourceView depth_texture_copy_srv_;
        direct3d::Texture2D normals_texture_copy_;
        direct3d::ShaderResourceView normals_texture_copy_srv_;
        std::shared_ptr<core::Window> window_ = nullptr;
    };
}