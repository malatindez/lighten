#pragma once
#include "../../components/collider.hpp"
#include "../gui/ui-helpers.hpp"

namespace lighten::gui
{
    template<>
    inline void DrawComponent<components::BoxCollider>(entt::registry &, entt::entity, std::string &entity_id, components::BoxCollider& collider)
    {
        if (ImGui::CollapsingHeader("Box Collider"))
        {
            ImGui::Text("Half extents: ");
            ImGui::SameLine();
            ImGui::DragFloat3(("##half-extents" + entity_id).c_str(), &collider.half_extents.x, 0.1f, 0.0f, 100.0f, "%.3f");
        }
    }

    template<>
    inline void DrawComponent<components::SphereCollider>(entt::registry &, entt::entity, std::string &entity_id,components::SphereCollider& collider)
    {
        if (ImGui::CollapsingHeader("Sphere Collider"))
        {
            ImGui::Text("Radius: ");
            ImGui::SameLine();
            ImGui::DragFloat(("##radius" + entity_id).c_str(), &collider.radius, 0.1f, 0.0f, 100.0f, "%.3f");
        }
    }
}
