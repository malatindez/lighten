#pragma once
#include "core/layers/render-pipeline.hpp"
#include "window.hpp"
namespace engine::platform::windows
{
    class RenderPipeline : public core::RenderPipeline
    {
    public:
        RenderPipeline();
        [[nodiscard]] core::math::vec4 &sky_color() noexcept { return sky_color_; }
        [[nodiscard]] core::math::vec4 const &sky_color() const noexcept { return sky_color_; }
        [[nodiscard]] std::shared_ptr<Window> window() const noexcept { return window_; }
        void OnRender() override;
        void OnUpdate() override;
    private:
        void FrameBegin();
        void FrameEnd();
        core::math::vec4 sky_color_ { core::math::vec3{0.25f}, 0.0f };
        engine::direct3d::RasterizerState1 rasterizer_state_;
        engine::direct3d::SamplerState sampler_state_;
        engine::direct3d::DepthStencilState depth_stencil_state_;
        std::shared_ptr<Window> window_;
    };
}