#pragma once
#include "../../components/force-fields.hpp"
#include "../ui-helpers.hpp"

namespace lighten::gui
{
    template<>
    inline void DrawComponent<components::PointForceField>(entt::registry &, entt::entity,  std::string &entity_id, components::PointForceField& force_field)
    {
        if (ImGui::CollapsingHeader("Point Force Field"))
        {
            ImGui::Text("Force: ");
            ImGui::SameLine();
            ImGui::DragFloat(("##force" + entity_id).c_str(), &force_field.force, 0.1f, -100.0f, 100.0f, "%.3f");
            ImGui::Text("Radius: ");
            ImGui::SameLine();
            ImGui::DragFloat(("##radius" + entity_id).c_str(), &force_field.radius, 0.1f, 0.0f, 100.0f, "%.3f");
            ImGui::Text("Falloff: ");
            ImGui::SameLine();
            ImGui::DragFloat(("##falloff" + entity_id).c_str(), &force_field.falloff, 0.1f, 0.0f, 100.0f, "%.3f");
        }
    }
    
    template<>
    inline void DrawComponent<components::DirectionalForceField>(entt::registry &, entt::entity,  std::string &entity_id, components::DirectionalForceField& force_field)
    {
        if (ImGui::CollapsingHeader("Directional Force Field"))
        {
            ImGui::Text("Force: ");
            ImGui::SameLine();
            ImGui::DragFloat(("##force" + entity_id).c_str(), &force_field.force, 0.1f, -100.0f, 100.0f, "%.3f");
            ImGui::Text("Radius: ");
            ImGui::SameLine();
            ImGui::DragFloat(("##radius" + entity_id).c_str(), &force_field.radius, 0.1f, 0.0f, 100.0f, "%.3f");
            ImGui::Text("Falloff: ");
            ImGui::SameLine();
            ImGui::DragFloat(("##falloff" + entity_id).c_str(), &force_field.falloff, 0.1f, 0.0f, 100.0f, "%.3f");
        }
    }

    template<>
    inline void DrawComponent<components::OmniDirectionalForceField>(entt::registry &, entt::entity,  std::string &entity_id, components::OmniDirectionalForceField& force_field)
    {
        if (ImGui::CollapsingHeader("Omni Directional Force Field"))
        {
            ImGui::Text("Force: ");
            ImGui::SameLine();
            ImGui::DragFloat(("##force" + entity_id).c_str(), &force_field.force, 0.1f, -100.0f, 100.0f, "%.3f");
            ImGui::Text("Radius: ");
            ImGui::SameLine();
            ImGui::DragFloat(("##radius" + entity_id).c_str(), &force_field.radius, 0.1f, 0.0f, 100.0f, "%.3f");
            ImGui::Text("Falloff: ");
            ImGui::SameLine();
            ImGui::DragFloat(("##falloff" + entity_id).c_str(), &force_field.falloff, 0.1f, 0.0f, 100.0f, "%.3f");
        }
    }
} // namespace lighten::gui