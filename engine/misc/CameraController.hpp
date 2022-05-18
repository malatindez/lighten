#pragma once
#include "components/camera.hpp"
#include "components/transform.hpp"
#include "entt/entt.hpp"
#include <iostream>
namespace engine
{
    class CameraController
    {
        public:
        CameraController(components::Camera &camera, components::Transform &transform, math::ivec2 const&size) : camera_(camera), transform_(transform), size(size)
        {
            camera_.view = math::lookAt(
                transform_.position, math::vec3{0,0,-2}, math::vec3{0,1,0});
            camera_.projection = math::perspective(math::radians(45.0f), float(size.x) / size.y, 0.02f, 20.00f);
            camera_.view_projection = camera_.projection * camera_.view;
            camera_.inv_view = math::inverse(camera_.view);
            camera_.inv_projection = math::inverse(camera_.projection);
            camera_.inv_view_projection = math::inverse(camera_.view_projection);;
        }
        void OnEvent(Event &event) {}
        components::Camera &camera_;
        components::Transform &transform_;
        math::ivec2 const& size;
    };
}