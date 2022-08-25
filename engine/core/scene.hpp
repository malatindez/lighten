#pragma once
#include "misc/camera-controller.hpp"
namespace engine::core
{
    class Scene
    {
    public:
        std::vector<entt::entity> FetchEntitiesByTag(std::string_view const tag)
        {
            std::vector<entt::entity> rv;
            for (auto entity : registry.view<components::TagComponent>())
            {
                auto &tag_component = registry.get<components::TagComponent>(entity);
                if (tag_component.tag == tag)
                {
                    rv.push_back(entity);
                }
            }
            return rv;
        }
        entt::registry registry;
        std::unique_ptr<engine::CameraController> main_camera = nullptr;
    };
}