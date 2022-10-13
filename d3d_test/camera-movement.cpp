#include "camera-movement.hpp"
using namespace engine;
using namespace core;
using namespace events;
using namespace math;
using namespace components;

namespace camera_movement
{
    ivec2 lb_saved_mouse_position{ -1 };
    ivec2 rb_saved_mouse_position{ -1 };

    std::shared_ptr<engine::core::Scene> selected_scene = nullptr;
    entt::entity selected_entity = entt::null;
    float selected_distance = 0.0f;
    vec3 selected_object_offset{ 0.0f };

    void RegisterKeyCallbacks()
    {
        auto input = InputLayer::instance();
        input->AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_W },
            [&] (InputLayer::KeySeq const &, uint32_t)
            { Engine::scene()->main_camera->flags() |= CameraController::MoveForward; });
        input->AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_S },
            [&] (InputLayer::KeySeq const &, uint32_t)
            { Engine::scene()->main_camera->flags() |= CameraController::MoveBackwards; });
        input->AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_A },
            [&] (InputLayer::KeySeq const &, uint32_t)
            { Engine::scene()->main_camera->flags() |= CameraController::MoveLeft; });
        input->AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_D },
            [&] (InputLayer::KeySeq const &, uint32_t)
            { Engine::scene()->main_camera->flags() |= CameraController::MoveRight; });
        input->AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_Q },
            [&] (InputLayer::KeySeq const &, uint32_t)
            { Engine::scene()->main_camera->flags() |= CameraController::RotateLeft; });
        input->AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_E },
            [&] (InputLayer::KeySeq const &, uint32_t)
            { Engine::scene()->main_camera->flags() |= CameraController::RotateRight; });
        input->AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_SHIFT },
            [&] (InputLayer::KeySeq const &, uint32_t)
            { Engine::scene()->main_camera->flags() |= CameraController::Accelerate; });
        input->AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_LBUTTON },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max() || InputLayer::instance()->key_state(engine::core::Key::KEY_CONTROL))
                {
                    lb_saved_mouse_position = core::math::ivec2{ -1 };
                    return;
                }
                if (lb_saved_mouse_position == core::math::vec2{ -1 })
                {
                    lb_saved_mouse_position = mouse_position();
                }
            });
        input->AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_RBUTTON },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max() || InputLayer::instance()->key_state(engine::core::Key::KEY_CONTROL))
                {
                    rb_saved_mouse_position = core::math::ivec2{ -1 };
                    return;
                }
                if (rb_saved_mouse_position == core::math::vec2{ -1 })
                {
                    auto &input = *InputLayer::instance();
                    auto scene = Engine::scene();
                    Ray ray = scene->main_camera->PixelRaycast(vec2{ input.mouse_position() });
                    Intersection nearest;
                    nearest.reset();
                    std::optional<entt::entity> entity = render::ModelSystem::FindIntersection(scene->registry, ray, nearest);
                    if (entity.has_value())
                    {
                        rb_saved_mouse_position = mouse_position();
                        selected_entity = entity.value();
                        selected_scene = scene;
                        selected_distance = length(nearest.point - scene->main_camera->position()) * dot(scene->main_camera->forward(), ray.direction());
                        selected_object_offset = scene->registry.get<TransformComponent>(selected_entity).position - nearest.point;
                    }
                    else
                    {
                        rb_saved_mouse_position = core::math::ivec2{ -1 };
                        selected_entity = entt::null;
                        selected_scene = nullptr;
                        selected_distance = 0;
                    }
                }
                else if (selected_scene)
                {
                    auto &input = *InputLayer::instance();
                    auto scene = Engine::scene();
                    Ray b = scene->main_camera->PixelRaycast(vec2{ input.mouse_position() });
                    rb_saved_mouse_position = input.mouse_position();
                    vec3 obj_offset = b.direction() * selected_distance / dot(scene->main_camera->forward(), b.direction());
                    auto &transform = scene->registry.get<TransformComponent>(selected_entity);
                    transform.position = selected_object_offset + obj_offset + scene->main_camera->position();
                    transform.UpdateMatrices();
                    render::ModelSystem::instance().OnInstancesUpdated(scene->registry);
                }
            });
    }
    void UpdateCamera(float delta_time)
    {
        auto &input = *InputLayer::instance();
        auto scene = Engine::scene();
        ivec2 pixel_delta{ 0 };
        if (lb_saved_mouse_position != ivec2{ -1 } && input.lbutton_down() && !InputLayer::instance()->key_state(engine::core::Key::KEY_CONTROL))
        {
            pixel_delta = input.mouse_position() - lb_saved_mouse_position;
        }
        scene->main_camera->OnTick(delta_time, pixel_delta);
    }
}