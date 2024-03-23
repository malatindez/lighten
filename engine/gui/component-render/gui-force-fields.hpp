#pragma once
#include "../../components/force-fields.hpp"
#include "../gui/ui-helpers.hpp"

namespace lighten::gui
{
    template<>
    void DrawComponent<components::PointForceField>(entt::registry &registry, entt::entity entity, std::string &entity_id, components::PointForceField& force_field)
    {
        if (ImGui::CollapsingHeader(("Point Force Field##" + entity_id).c_str())
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
    void DrawComponent<components::DirectionalForceField>(entt::registry &registry, entt::entity entity, std::string &entity_id, components::DirectionalForceField& force_field)
    {
        if (ImGui::CollapsingHeader(("Directional Force Field##" + entity_id).c_str()))
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
    void DrawComponent<components::OmniDirectionalForceField>(entt::registry &registry, entt::entity entity, std::string &entity_id, components::OmniDirectionalForceField& force_field)
    {
        if (ImGui::CollapsingHeader(("Omni Directional Force Field##" + entity_id).c_str()))
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