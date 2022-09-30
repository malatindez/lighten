#pragma once
#include "renderer.hpp"
#include "misc/camera-controller.hpp"
class Controller
    : public engine::core::Layer,
    public engine::core::Layer::HandleTick,
    public engine::core::Layer::HandleEvent,
    public engine::core::Layer::HandleGuiRender
{
public:
    using UpdateCallback = std::function<void(float)>;
    using engine::core::Layer::Layer;
    entt::entity main_camera_entity;
    entt::entity last_created_knight;

    Controller(std::shared_ptr<Renderer> renderer, engine::core::math::ivec2 const &window_size, engine::core::math::ivec2 const &window_pos, float &exposure);
    std::vector<std::function<void(float)>> &update_callbacks() { return update_callbacks_; }
    void OnTick(float delta_time) override;
    void OnEvent(engine::core::events::Event &e) override;
    void OnGuiRender() override;

private:
    std::shared_ptr<engine::core::Scene> first_scene;
private:
    float &exposure_;
    engine::core::math::ivec2 const &window_size;
    engine::core::math::ivec2 const &window_pos;
    std::vector<UpdateCallback> update_callbacks_;
    std::shared_ptr<Renderer> renderer_;
};