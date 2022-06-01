#pragma once
#include "core/bitmap-window.hpp"
#include "core/layer.hpp"
#include "core/parallel-executor.hpp"
#include "core/window.hpp"
#include "input.hpp"
#include "math.hpp"
#include "misc/camera-controller.hpp"
#include "misc/scene.hpp"

#include <memory>
#include <unordered_map>

class Controller : public engine::Layer
{
public:
    Controller(engine::BitmapWindow &window, std::shared_ptr<engine::Scene> scene, engine::CameraController cam);

    void OnEvent(engine::Event &event) override;
    [[nodiscard]] engine::CameraController const &camera_controller() const noexcept { return camera_controller_; }

private:
    void Tick(float delta_time);
    engine::CameraController camera_controller_;
    Input input_;
    std::shared_ptr<engine::Scene> scene_;
    engine::BitmapWindow &window_;
    engine::math::ivec2 saved_mouse_position_{0};
    engine::ParallelExecutor executor;
};