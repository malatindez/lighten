#include "controller.hpp"
using namespace engine;
using namespace core;
using namespace events;
using namespace math;
using namespace components;
Controller::Controller(std::shared_ptr<Renderer> renderer, math::ivec2 const &window_size) : current_mouse_position_(InputLayer::instance()->mouse_position()), renderer_{ renderer }
{
    first_scene = std::make_shared<Scene>();

    auto &registry = first_scene->registry;
    auto main_camera = registry.create();
    auto &camera = registry.emplace<CameraComponent>(main_camera, CameraComponent());
    auto &camera_transform = registry.emplace<TransformComponent>(main_camera, TransformComponent());
    registry.emplace<TagComponent>(main_camera, TagComponent{ .tag = "Main Camera" });
    first_scene->main_camera = std::make_unique<CameraController>(&registry, main_camera, window_size);
    Engine::SetScene(first_scene);
    int amount = 22;
    for (int i = 0; i < amount; i++)
    {
        auto knight = registry.create();
        auto &transform = registry.emplace<TransformComponent>(knight);
        registry.emplace<OpaqueComponent>(knight);
        registry.emplace<ModelComponent>(knight).model_id = renderer->knight_model_id;
        transform.position = vec3
        {
            std::sin(float(i) / amount * 2 * (float)std::numbers::pi),
            0,
            std::cos(float(i) / amount * 2 * (float)std::numbers::pi)
        } *(amount / std::sqrtf(amount));
        transform.rotation = QuaternionFromEuler(radians(180.0f), 0.0f, radians(180.0f));
        transform.rotation *= QuaternionFromRotationMatrix(lookAt(transform.position, vec3{ 0,0,0 }, vec3{ 0,1,0 }).as_rmat<3, 3>());
        transform.UpdateMatrices();
    }
    render::ModelSystem::instance()->OnInstancesUpdated(registry);
    auto input = InputLayer::instance();

    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_W },
        [this](InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveForward; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_S },
        [this](InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveBackwards; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_A },
        [this](InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveLeft; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_D },
        [this](InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveRight; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_SPACE },
        [this](InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveUp; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_CONTROL },
        [this](InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveDown; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_Q },
        [this](InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::RotateLeft; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_E },
        [this](InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::RotateRight; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_SHIFT },
        [this](InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::Accelerate; });
}
void Controller::OnTick([[maybe_unused]] float delta_time)
{
    for (auto const &func : update_callbacks_)
    {
        func(delta_time);
    }
    ivec2 pixel_delta{ 0 };
    if (InputLayer::instance()->lbutton_down())
    {
        if (previous_mouse_position == core::math::vec2{ -1, -1 })
        {
            previous_mouse_position = current_mouse_position_;
        }
        pixel_delta = current_mouse_position_ - previous_mouse_position;
    }
    else
    {
        previous_mouse_position = core::math::ivec2{ -1, -1 };
    }
    first_scene->main_camera->OnTick(delta_time, pixel_delta);

    renderer_->per_frame.view_projection = first_scene->main_camera->camera().view_projection;
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