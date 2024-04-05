#pragma once
#include "../../components/scene.hpp"
#include "../ui-helpers.hpp"
namespace lighten::gui
{
    template<>
    inline void DrawComponent<components::Scene>(entt::registry &, entt::entity , std::string &, components::Scene&)
    {
        ImGui::Text("Scene");
    }
}