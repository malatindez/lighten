#pragma once
#include "camera-movement.hpp"
using namespace engine;
using namespace core;
using namespace events;
using namespace math;
using namespace components;

namespace camera_movement
{
    ivec2 previous_mouse_position{ -1 };

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
            InputLayer::KeySeq{ engine::core::Key::KEY_SPACE },
            [&] (InputLayer::KeySeq const &, uint32_t)
            { Engine::scene()->main_camera->flags() |= CameraController::MoveUp; });
        input->AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_CONTROL },
            [&] (InputLayer::KeySeq const &, uint32_t)
            { Engine::scene()->main_camera->flags() |= CameraController::MoveDown; });
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
            InputLayer::KeySeq{ engine::core::Key::KEY_RBUTTON },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    previous_mouse_position = core::math::ivec2{ -1 };
                    return;
                }
                if (previous_mouse_position == core::math::vec2{ -1 })
                {
                    previous_mouse_position = mouse_position();
                }
            });
    }
    void UpdateCamera(float delta_time)
    {
        auto &input = *InputLayer::instance();
        auto scene = Engine::scene();
        ivec2 pixel_delta{ 0 };
        if (input.rbutton_down())
        {
            pixel_delta = input.mouse_position() - previous_mouse_position;
        }
        scene->main_camera->OnTick(delta_time, pixel_delta);
    }
}