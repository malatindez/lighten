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
        [[nodiscard]] inline engine::render::PerFrame const &per_frame() const noexcept { return per_frame_; }
        [[nodiscard]] inline engine::render::PerFrame &per_frame() noexcept { return per_frame_; }
        void OnRender() override;
        void OnUpdate() override;

    private:
        void FrameBegin() override;
        void FrameEnd() override;
        core::math::vec4 sky_color_{ core::math::vec3{0.25f}, 0.0f };
        std::shared_ptr<Window> window_;
        render::PerFrame per_frame_;
        direct3d::DynamicUniformBuffer<engine::render::PerFrame> per_frame_buffer_{};
    };
}