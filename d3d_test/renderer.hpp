#pragma once
#pragma once
#include "core/engine.hpp"
#include "render/render.hpp"
struct TestCubeComponent {};
class Renderer
    : public engine::core::Layer,
    public engine::core::Layer::HandleRender,
    public engine::core::Layer::HandleGuiRender
{
public:
    Renderer();
    void OnRender() override;
    void OnGuiRender() override;

    engine::render::PerFrame per_frame;
    engine::direct3d::DynamicUniformBuffer<engine::render::PerFrame> per_frame_buffer{};
    uint32_t knight_model_id, cube_model_id;
    engine::direct3d::SamplerState current_state;
private:
    engine::render::GraphicsShaderProgram test_shader;
    void DrawTestCube();
};