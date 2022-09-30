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
    Renderer(engine::core::math::ivec2 const &screen_resolution, engine::render::PerFrame &per_frame);
    void OnRender() override;
    void OnGuiRender() override;

    engine::render::PerFrame &per_frame;
private:
    engine::core::math::ivec2 const &screen_resolution;
    engine::render::GraphicsShaderProgram test_shader;
};