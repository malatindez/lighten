#pragma once
#include "core/bitmap-window.hpp"
#include "core/layer.hpp"
#include "core/parallel-executor.hpp"
#include "core/window.hpp"
#include "input.hpp"
#include "core/math.hpp"
#include "misc/camera-controller.hpp"

#include "misc/scene.hpp"
#include "render/cube.hpp"
#include <memory>
#include <stdint.h>
#include <unordered_map>

class Controller : public engine::core::Layer
{
public:
    Controller(engine::core::BitmapWindow &window, std::shared_ptr<engine::Scene> scene, engine::CameraController const &cam);

    void OnEvent(engine::core::events::Event &event) override;

    [[nodiscard]] engine::CameraController const &camera_controller() const noexcept { return camera_controller_; }

    [[nodiscard]] float const &roll_speed() const noexcept { return roll_speed_; }
    [[nodiscard]] float const &move_speed() const noexcept { return move_speed_; }
    [[nodiscard]] float const &sensivity() const noexcept { return sensivity_; }
    [[nodiscard]] float &roll_speed() noexcept { return roll_speed_; }
    [[nodiscard]] float &move_speed() noexcept { return move_speed_; }
    [[nodiscard]] float &sensivity() noexcept { return sensivity_; }

    std::vector<std::function<void(float)>> &update_callbacks() { return update_callbacks_; }

private:
    void InitScene();
    void InitInput();
    void OnMoveCallback(float, Input::KeySeq const &, uint32_t);
    void Tick(float delta_time);

    [[nodiscard]] inline engine::core::math::Ray PixelRaycast(engine::core::math::vec2 ndc) const noexcept
    {
        ndc /= window_.window_size();
        ndc = ndc * 2.0f - 1.0f;
        ndc.v = -ndc.v;
        return camera_controller_.Raycast(ndc);
    }
    
    float roll_speed_ = engine::core::math::radians(60.0f);
    float move_speed_ = 2.0f;
    float sensivity_ = 8.0f;
    engine::core::math::ivec2 lb_saved_mouse_position_{0};
    engine::core::math::ivec2 rb_saved_mouse_position_{0};
    engine::components::Transform *selected_object_ = nullptr;
    float selected_object_distance_ = 0.0f;
    engine::core::math::vec3 selected_object_offset_{0.0f};

    engine::CameraController camera_controller_;
    Input input_;
    std::shared_ptr<engine::Scene> scene_;
    engine::core::BitmapWindow &window_;
    engine::core::ParallelExecutor executor{
        static_cast<uint32_t>(std::max(1, std::max(int32_t(engine::core::ParallelExecutor::kMaxThreads) - 4, int32_t(engine::core::ParallelExecutor::kHalfThreads))))};
    std::vector<std::function<void(float)>> update_callbacks_;
    long double time_from_start_ = 0;
};