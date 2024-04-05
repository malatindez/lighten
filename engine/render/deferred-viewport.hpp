#pragma once
#include "../direct3d11/direct3d11.hpp"
#include "common.hpp"
#include "hdr-to-ldr-layer.hpp"
#include "deferred-resolve.hpp"
#include "present-swapchain.hpp"
#include "renderer.hpp"

#include "../subsystems/render/post-processing.hpp"
#include "../core/layers/layer-stack.hpp"
#include "../components/components.hpp"
#include "../subsystems/core/input-layer.hpp"
#include "../subsystems/render/skybox-manager.hpp"
#include "../core/engine.hpp"
#include "abstract/viewport-base.hpp"

namespace lighten::render
{
    class DeferredViewport : public ViewportBase
    {
    public:
        DeferredViewport(std::string viewport_name, glm::ivec2 const &initial_size, std::shared_ptr<core::World> world);

        [[nodiscard]] inline glm::vec4 &sky_color() noexcept { return sky_color_; }
        [[nodiscard]] inline glm::vec4 const &sky_color() const noexcept { return sky_color_; }
        [[nodiscard]] inline render::PerFrame const &per_frame() const noexcept { return per_frame_; }
        [[nodiscard]] inline render::PerFrame &per_frame() noexcept { return per_frame_; }
        [[nodiscard]] inline std::shared_ptr<render::HDRtoLDRLayer> hdr_to_ldr_layer() const noexcept { return hdr_to_ldr_layer_; }
        [[nodiscard]] inline auto deferred_resolve() noexcept { return deferred_resolve_; }

        void OnRender() override;
        void OnUpdate() override;
        void OnTick(float) override;
        void OnEvent(core::events::Event &) override;
        
        void OnResize(glm::ivec2 const &new_size) override;
        void* GetResourceView() override { return (void*)output_render_texture_.shader_resource_view(); }

        render::Renderer &renderer() noexcept { return *renderer; }
    private:
        void FrameBegin();
        void PostProcess();
        void FrameEnd();
        
        std::unique_ptr<render::Renderer> renderer = nullptr;
        lighten::CameraController main_camera = nullptr;
        
        glm::ivec2 viewport_size_;
        CameraController camera_controller_;

        direct3d::RenderTarget hdr_target_;
        direct3d::RenderTarget output_render_texture_;

        mal_toolkit::HighResolutionTimer timer;
        glm::vec4 sky_color_{glm::vec3{0.25f}, 0.0f};
        

        [[maybe_unused]] D3D11_VIEWPORT viewport_;
        render::PerFrame per_frame_;
        direct3d::DynamicUniformBuffer<lighten::render::PerFrame> per_frame_buffer_{};


        direct3d::DepthStencil depth_stencil_;
        std::shared_ptr<direct3d::SwapchainRenderTarget> output_target_;


        render::GBuffer gbuffer_;

        std::shared_ptr<render::DeferredResolve> deferred_resolve_ = nullptr;
        std::shared_ptr<render::HDRtoLDRLayer> hdr_to_ldr_layer_ = nullptr;
        std::shared_ptr<render::PresentSwapchain> swapchain_present_ = nullptr;
        direct3d::Texture2D depth_texture_copy_;
        direct3d::ShaderResourceView depth_texture_copy_srv_;
        direct3d::Texture2D normals_texture_copy_;
        direct3d::ShaderResourceView normals_texture_copy_srv_;
    };
}