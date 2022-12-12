#pragma once
#include "../subsystems/render/post-processing.hpp"
#include "shader-program.hpp"
#include "../subsystems/core/shader-manager.hpp"
namespace engine::render
{
    class PresentSwapchain
        : public PostProcessingLayer,
        public PostProcessingLayer::HandleFrameEnd
    {
    public:
        PresentSwapchain() : PostProcessingLayer(std::numeric_limits<int64_t>::max()) {}
        void OnFrameEnd(direct3d::RenderTargetBase &render_target) override
        {
            static utils::Measurer<std::chrono::high_resolution_clock> measurer{ "swapchain.present" };
            measurer.begin();
            static_cast<direct3d::SwapchainRenderTarget &>(render_target).swapchain()->Present(0, 0);
            measurer.end();
        }
        uint32_t sync_interval = 0;
        uint32_t flags = 0;
    };
}