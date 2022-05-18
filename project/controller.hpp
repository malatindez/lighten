#pragma once
#include "misc/scene.hpp"
#include "core/window.hpp"
#include "core/bitmap-window.hpp"
#include "core/layer.hpp"
#include "input.hpp"
#include "misc/CameraController.hpp"

#include <memory>
#include <unordered_map>

class Controller : public engine::Layer
{
public:
    Controller(engine::BitmapWindow &window, std::shared_ptr<engine::Scene> scene, engine::CameraController cam);
   
    void OnEvent(engine::Event &event) override;
 
private:
    void Tick(float delta_time) const;
    engine::CameraController camera_controller_;
    Input input_;
    std::shared_ptr<engine::Scene> scene_;
    engine::BitmapWindow &window_;
};