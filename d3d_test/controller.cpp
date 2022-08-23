#include "controller.hpp"
using namespace engine;
using namespace core;
using namespace events;
using namespace math;

Controller::Controller(std::shared_ptr<Renderer> renderer, math::ivec2 const &window_size) : current_mouse_position_(InputLayer::instance()->mouse_position()), renderer_ { renderer }
{
    camera_controller_ = std::make_unique<CameraController>(
        &camera_, &camera_transform_, window_size);
    auto input = InputLayer::instance();
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq { engine::core::Key::KEY_W },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { camera_controller_->flags() |= CameraController::MoveForward; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq { engine::core::Key::KEY_S },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { camera_controller_->flags() |= CameraController::MoveBackwards; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq { engine::core::Key::KEY_A },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { camera_controller_->flags() |= CameraController::MoveLeft; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq { engine::core::Key::KEY_D },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { camera_controller_->flags() |= CameraController::MoveRight; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq { engine::core::Key::KEY_SPACE },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { camera_controller_->flags() |= CameraController::MoveUp; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq { engine::core::Key::KEY_CONTROL },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { camera_controller_->flags() |= CameraController::MoveDown; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq { engine::core::Key::KEY_Q },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { camera_controller_->flags() |= CameraController::RotateLeft; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq { engine::core::Key::KEY_E },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { camera_controller_->flags() |= CameraController::RotateRight; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq { engine::core::Key::KEY_SHIFT },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { camera_controller_->flags() |= CameraController::Accelerate; });
}
void Controller::OnTick([[maybe_unused]] float delta_time)
{
    for (auto const &func : update_callbacks_)
    {
        func(delta_time);
    }
    ivec2 pixel_delta { 0 };
    if (InputLayer::instance()->lbutton_down())
    {
        if (previous_mouse_position == core::math::vec2 { -1, -1 })
        {
            previous_mouse_position = current_mouse_position_;
        }
        pixel_delta = current_mouse_position_ - previous_mouse_position;
    }
    else
    {
        previous_mouse_position = core::math::ivec2 { -1, -1 };
    }
    camera_controller_->OnTick(delta_time, pixel_delta);

    renderer_->per_frame.view_projection = camera_.view_projection;
    /*if (input_.rbutton_down() && selected_object_)
    {
        Ray a = PixelRaycast(vec2{rb_saved_mouse_position_});
        Ray b = PixelRaycast(vec2{input_.mouse_position()});
        rb_saved_mouse_position_ = input_.mouse_position();

        vec3 obj_offset = b.PointAtParameter(selected_object_distance_);

        selected_object_->position = selected_object_offset_ + obj_offset;
        selected_object_->UpdateMatrices();
    }
    if (input_.mouse_scrolled())
    {
        if (selected_object_ && input_.rbutton_down())
        {
            selected_object_->scale *= vec3{powf(math::clamp(1 + delta_time / 120 * input_.scroll_delta(), 0.5f, 1.5f), 0.5f)};
            rclamp(selected_object_->scale, 0.1f, std::numeric_limits<float>::max());
        }
        else
        {
            move_speed_ = std::max(0.01f, move_speed_ * (input_.scroll_delta() > 0 ? 1.1f : 1.0f / 1.1f));
        }
    }*/
}
void Controller::OnEvent(engine::core::events::Event &e)
{
    if (e.type() == engine::core::events::EventType::WindowClose) [[unlikely]]
    {
        engine::core::Engine::Exit();
    }
}