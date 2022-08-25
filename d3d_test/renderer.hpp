#pragma once
#pragma once
#include "core/engine.hpp"
#include "render/render.hpp"
class Renderer
    : public engine::core::Layer,
    public engine::core::Layer::HandleRender
{
public:
    Renderer();
    void OnRender() override;

    engine::render::PerFrame per_frame;
    engine::direct3d::DynamicUniformBuffer<engine::render::PerFrame> per_frame_buffer{};
    uint32_t knight_model_id;
};