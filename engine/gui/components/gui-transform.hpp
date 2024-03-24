#pragma once
#include "../../components/transform.hpp"
#include "../gui/ui-helpers.hpp"
namespace lighten::gui
{
    template<>
    inline void DrawComponent<components::Transform>(entt::registry &registry, entt::entity entity, std::string &entity_id, components::Transform& transform)
    {
        if (ImGui::CollapsingHeader(("Transform##" + entity_id).c_str()))
        {
            ImGui::Text("Position: ");
            ImGui::SameLine();
            bool changed = ImGui::DragFloat3(("##position" + entity_id).c_str(), &transform.position.x, 0.1f, -1000.0f, 1000.0f, "%.3f");
            ImGui::Text("Rotation: ");
            ImGui::SameLine();
            changed |= ImGui::DragFloat3(("##rotation" + entity_id).c_str(), &transform.rotation.x, 0.1f, -360.0f, 360.0f, "%.3f");
            ImGui::Text("Scale: ");
            ImGui::SameLine();
            changed |= ImGui::DragFloat3(("##scale" + entity_id).c_str(), &transform.scale.x, 0.1f, 0.0f, 1000.0f, "%.3f");
            if (changed)
            {
                registry.patch<components::Transform>(entity, [&transform](components::Transform &) {});
            }
        }
    }
    template<>
    inline void DrawComponent<components::WorldTransform>(entt::registry &, entt::entity, std::string &entity_id, components::WorldTransform& transform)
    {
        if (ImGui::CollapsingHeader(("WorldTransform##" + entity_id).c_str()))
        {
            ImGui::Text("World matrix: ");
            std::string world = FormatMatrixToString(transform.world);
            ImGui::Text("%s", world.c_str());
            ImGui::Text("Inv world matrix: ");
            std::string inv_world = FormatMatrixToString(transform.inv_world);
            ImGui::Text("%s", inv_world.c_str());
            ImGui::Text("Model matrix: ");
            std::string model = FormatMatrixToString(transform.model);
            ImGui::Text("%s", model.c_str());
            ImGui::Text("Inv model matrix: ");
            std::string inv_model = FormatMatrixToString(transform.inv_model);
            ImGui::Text("%s", inv_model.c_str());
        }
    }
}