#pragma once
#include "core/engine.hpp"
#include "render/render.hpp"
struct TestCubeComponent {
    uint64_t model_id;
};
class Renderer
    : public engine::core::Layer,
    public engine::core::Layer::HandleRender,
    public engine::core::Layer::HandleGuiRender
{
public:
    Renderer(engine::core::math::ivec2 const &screen_resolution);
    void OnRender() override;
    void OnGuiRender() override;

    engine::render::PerFrame per_frame;
    engine::direct3d::DynamicUniformBuffer<engine::render::PerFrame> per_frame_buffer{};
    engine::direct3d::SamplerState current_state;
private:
    engine::core::math::ivec2 const &screen_resolution;
    engine::render::GraphicsShaderProgram test_shader;
    void DrawTestCube();
};