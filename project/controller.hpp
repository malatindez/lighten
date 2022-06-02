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

    [[nodiscard]] inline engine::math::Ray PixelRaycast(engine::math::vec2 uv) const noexcept
    {
        uv /= window_.window_size();
        uv = uv * 2 - 1;
        uv.v = -uv.v;
        return camera_controller_.Raycast(uv);
    }

    engine::CameraController camera_controller_;
    Input input_;
    std::shared_ptr<engine::Scene> scene_;
    engine::BitmapWindow &window_;
    engine::math::ivec2 lb_saved_mouse_position_;
    engine::math::ivec2 rb_saved_mouse_position_;
    engine::components::Transform *selected_object = nullptr;
    engine::ParallelExecutor executor;
};