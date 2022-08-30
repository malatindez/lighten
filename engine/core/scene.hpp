#pragma once
#include "misc/camera-controller.hpp"
namespace engine::core
{
    class Scene
    {
    public:
        entt::registry registry;
        std::unique_ptr<engine::CameraController> main_camera = nullptr;
    };
}