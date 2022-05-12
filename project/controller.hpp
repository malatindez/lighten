#pragma once
#include "misc/scene.hpp"
#include "core/window.hpp"
#include "core/layer.hpp"
#include "input.hpp"

#include <memory>
#include <unordered_map>

class Controller : public engine::Layer
{
public:
    Controller(engine::BitmapWindow &window, std::shared_ptr<engine::Scene> scene);
   
    void OnEvent(engine::Event &event) override;
 
private:
    void Tick(float delta_time) const;

    Input input_;
    std::shared_ptr<engine::Scene> scene_;
    engine::BitmapWindow &window_;
};