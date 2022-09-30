#pragma once
#include "../subsystems/render/post-processing.hpp"
#include "shader-program.hpp"
#include "../subsystems/core/shader-manager.hpp"
namespace engine::render
{
    class HDRtoLDRLayer
        : public PostProcessingLayer,
        public PostProcessingLayer::HandleProcess
    {
    public:
        struct Buffer
        {
            float exposure = 1.0f;
            core::math::vec3 padding;
        };
        HDRtoLDRLayer(direct3d::SwapchainRenderTarget &window_render_target) : PostProcessingLayer(0), window_render_target_{ window_render_target }
        {
            auto path = std::filesystem::current_path();
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / vs_shader_path);
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / ps_shader_path);
            shader_.SetVertexShader(vs).SetPixelShader(ps);
        }
        direct3d::RenderTargetBase &OnProcess(direct3d::RenderTargetBase &source) override
        {
            window_render_target_.SizeResources(source.size());
            direct3d::api().devcon4->OMSetRenderTargets(0, nullptr, nullptr);
            direct3d::api().devcon4->PSSetShaderResources(0, 1, &source.shader_resource_view());
            direct3d::api().devcon4->OMSetRenderTargets(1, &window_render_target_.render_target_view(), nullptr);
            shader_.Bind();
            constant_buffer_.Update(Buffer{ exposure_ , {0} });
            constant_buffer_.Bind(direct3d::ShaderType::PixelShader, 1);
            direct3d::api().devcon4->Draw(3, 0);
            ID3D11ShaderResourceView *temp = nullptr;
            direct3d::api().devcon4->PSSetShaderResources(0, 1, &temp);
            return window_render_target_;
        }
        [[nodiscard]] float &exposure() noexcept { return exposure_; }
        [[nodiscard]] float const &exposure() const noexcept { return exposure_; }

    private:
        static constexpr auto vs_shader_path = "assets/shaders/post-processing/hdr-to-ldr-vs.hlsl";
        static constexpr auto ps_shader_path = "assets/shaders/post-processing/hdr-to-ldr-ps.hlsl";
        float exposure_ = 1.0f;
        GraphicsShaderProgram shader_;
        direct3d::DynamicUniformBuffer<Buffer> constant_buffer_;
        direct3d::SwapchainRenderTarget &window_render_target_;
    };
}