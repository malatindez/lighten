#pragma once
#include "direct3d11/direct3d11.hpp"
#include "core/layers/render-pipeline.hpp"
#include "render/common.hpp"
#include "subsystems/render/post-processing.hpp"
#include "render/hdr-to-ldr-layer.hpp"
#include "render/deferred-resolve.hpp"
#include "render/present-swapchain.hpp"
#include "render/renderer.hpp"
namespace lighten::direct3d
{
    class DeferredHDRRenderPipeline : public core::RenderPipeline
    {
    public:
        DeferredHDRRenderPipeline(std::shared_ptr<core::Window> window, std::shared_ptr<SwapchainRenderTarget> const &output_target);

        void OnRender() override;
        void OnEvent(core::events::Event &) override;
        void WindowSizeChanged(glm::ivec2 const &size);

        glm::ivec2 const& framebuffer_size() const { return framebuffer_size_; }

    public:
        void FrameBegin() override;
        void FrameEnd() override;
        void PostProcess();

        glm::ivec2 framebuffer_size_;

        std::shared_ptr<direct3d::SwapchainRenderTarget> output_target_;
        D3D11_VIEWPORT viewport_;

        render::GBuffer gbuffer_;

        std::shared_ptr<core::Window> window_ = nullptr;
    };
}