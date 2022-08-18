#pragma once
#include "core/engine.hpp"
#include "misc/camera-controller.hpp"
#include "renderer.hpp"
#include "core/layers/input-layer.hpp"
class Controller : public engine::core::Layer
{
public:
    using UpdateCallback = std::function<void(float)>;
    using engine::core::Layer::Layer;
    Controller(std::shared_ptr<Renderer> renderer, engine::core::math::ivec2 const &window_size);
    std::vector<std::function<void(float)>> &update_callbacks() { return update_callbacks_; }
    void OnTick(float delta_time) override;
    void OnEvent(engine::core::events::Event &e);

private:
    engine::core::math::ivec2 const &current_mouse_position_;
private:
    engine::core::math::ivec2 previous_mouse_position { 0,0 };
    engine::components::Transform camera_transform_;
    engine::components::Camera camera_;
    std::unique_ptr<engine::CameraController> camera_controller_;
private:
    std::vector<UpdateCallback> update_callbacks_;
    std::shared_ptr<Renderer> renderer_;
};