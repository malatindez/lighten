#pragma once
#pragma once
#include "core/application.hpp"
#include "core/layer.hpp"
#include "core/swapchain-window.hpp"
#include "direct3d/common.hpp"
#include "direct3d/globals.hpp"
#include "direct3d/shader.hpp"
#include "direct3d/triangle-mesh.hpp"
class Renderer : public engine::core::Layer
{
public:
    struct Triangle
    {
        struct ShaderInput
        {
            engine::core::math::mat4 Transform;
            engine::core::math::mat4 Projection;
            engine::core::math::vec3 LightVector;
        };
        engine::direct3d::TriangleMesh mesh;
        engine::direct3d::ShaderProgram shader;
    };
    explicit Renderer(std::shared_ptr<engine::core::SwapchainWindow> window)
        : window_(window), triangle_(create_triangle())
    {
        initialize();
    }
    void OnEvent(engine::core::events::Event &event) override;

private:
    static Triangle create_triangle();

    void initialize();

    std::shared_ptr<engine::core::SwapchainWindow> window_;
    Triangle triangle_;
    engine::direct3d::RasterizerState1 rasterizer_state_;
    engine::direct3d::SamplerState sampler_state_;
    engine::direct3d::DepthStencilState depth_stencil_state_;
};