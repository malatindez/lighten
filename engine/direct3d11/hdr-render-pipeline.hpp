#pragma once
#include "direct3d11/direct3d11.hpp"
#include "core/layers/render-pipeline.hpp"
#include "render/common.hpp"
#include "subsystems/render/post-processing.hpp"
#include "render/hdr-to-ldr-layer.hpp"
#include "render/renderer.hpp"
namespace engine::direct3d
{
    class HDRRenderPipeline : public core::RenderPipeline
    {
    public:
        HDRRenderPipeline(std::shared_ptr<core::Window> window, std::shared_ptr<RenderTargetBase> const &output_target);
        HDRRenderPipeline(HDRRenderPipeline const &) = delete;
        HDRRenderPipeline(HDRRenderPipeline &&) = delete;
        HDRRenderPipeline &operator=(HDRRenderPipeline const &) = delete;
        HDRRenderPipeline &operator=(HDRRenderPipeline &&) = delete;

        [[nodiscard]] inline core::math::vec4 &sky_color() noexcept { return sky_color_; }
        [[nodiscard]] inline core::math::vec4 const &sky_color() const noexcept { return sky_color_; }
        [[nodiscard]] inline render::PerFrame const &per_frame() const noexcept { return per_frame_; }
        [[nodiscard]] inline render::PerFrame &per_frame() noexcept { return per_frame_; }
        [[nodiscard]] inline std::shared_ptr<render::HDRtoLDRLayer> hdr_to_ldr_layer() const noexcept { return hdr_to_ldr_layer_; }
        [[nodiscard]] inline render::PostProcessing const &post_processing() const noexcept { return *post_processing_; }
        [[nodiscard]] inline render::PostProcessing &post_processing() noexcept { return *post_processing_; }

        void OnRender() override;
        void OnUpdate() override;
        void OnEvent(core::events::Event &) override;
        void SetOutputTarget(std::shared_ptr<RenderTargetBase> const &target)
        {
            output_target_ = target;
            hdr_to_ldr_layer_->SetRenderTarget(target);
        }
        void WindowSizeChanged(core::math::ivec2 const &size);

    private:
        void FrameBegin() override;
        void PostProcess();
        void FrameEnd() override;
        core::math::vec4 sky_color_{ core::math::vec3{0.25f}, 0.0f };
        render::PerFrame per_frame_;
        DynamicUniformBuffer<engine::render::PerFrame> per_frame_buffer_{};
        DepthStencil depth_stencil_;
        RenderTarget hdr_target_;
        std::shared_ptr<RenderTargetBase> output_target_;
        D3D11_VIEWPORT viewport_;
        std::shared_ptr<render::HDRtoLDRLayer> hdr_to_ldr_layer_ = nullptr;
        std::shared_ptr<render::PostProcessing> post_processing_ = nullptr;
    };
}