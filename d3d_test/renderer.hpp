#pragma once
#pragma once
#include "core/engine.hpp"
#include "core/layers/shader-manager.hpp"
#include "core/shader-compiler.hpp"
#include "direct3d11/direct3d11.hpp"
#include "platform/windows/windows-render-pipeline.hpp"
#include "render/model.hpp"
#include "render/uniform-buffer.hpp"
class Renderer : public engine::core::Layer
{
public:
    Renderer();
    struct PerFrame
    {
        engine::core::math::mat4 model { 1 };
        engine::core::math::mat4 projection;
        engine::core::math::mat4 view;
    } per_frame;

    void OnRender() override;
    void OnGuiRender() override;

private:
    engine::render::UniformBuffer<PerFrame> per_frame_buffer {};
    engine::render::UniformBuffer<engine::core::math::mat4> per_model_buffer {};
    engine::render::UniformBuffer<engine::core::math::mat4> per_mesh_buffer {};
    std::shared_ptr<engine::render::Model> model;
    engine::render::GraphicsShaderProgram shader;
};