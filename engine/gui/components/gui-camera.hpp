#pragma once
#include "../../components/camera.hpp"
#include "../ui-helpers.hpp"
namespace lighten::gui
{
    template<>
    inline void DrawComponent<components::Camera>(entt::registry &registry, entt::entity entity, std::string &entity_id, components::Camera& camera)
    {
        if (ImGui::CollapsingHeader("Camera"))
        {
            bool changed = false;
            ImGui::Text("Field of view: ");
            ImGui::SameLine();
            changed |= ImGui::SliderFloat(("##fov" + entity_id).c_str(), &camera.fovy_, 0, 100, "%.3f");
            ImGui::Text("Near plane: ");
            ImGui::SameLine();
            changed |= ImGui::SliderFloat(("##near-plane" + entity_id).c_str(), &camera.z_near_, 0, 2000, "%.3f");
            ImGui::Text("Far plane: ");
            ImGui::SameLine();
            changed |= ImGui::SliderFloat(("##far-plane" + entity_id).c_str(), &camera.z_far_, 0, 2000, "%.3f");
            ImGui::Text("Aspect ratio: ");
            ImGui::SameLine();
            changed |= ImGui::SliderFloat(("##aspect-ratio" + entity_id).c_str(), &camera.aspect_ratio_, 0, 5, "%.3f");
            if (changed)
            {
                registry.patch<components::Camera>(entity, [&](components::Camera &) {});
            }
        }
        if (ImGui::CollapsingHeader("Camera data: "))
        {
            std::string view = FormatMatrixToString(camera.view);
            std::string projection = FormatMatrixToString(camera.view);
            std::string view_projection = FormatMatrixToString(camera.view);
            std::string inv_view = FormatMatrixToString(camera.view);
            std::string inv_projection = FormatMatrixToString(camera.view);
            std::string inv_view_projection = FormatMatrixToString(camera.view);
            ImGui::Text("View matrix: ");
            ImGui::Text("%s", view.c_str());
            ImGui::Text("Projection matrix: ");
            ImGui::Text("%s", projection.c_str());
            ImGui::Text("View projection matrix: ");
            ImGui::Text("%s", view_projection.c_str());
            ImGui::Text("Inv view matrix: ");
            ImGui::Text("%s", inv_view.c_str());
            ImGui::Text("Inv projection matrix: ");
            ImGui::Text("%s", inv_projection.c_str());
            ImGui::Text("Inv view projection matrix: ");
            ImGui::Text("%s", inv_view_projection.c_str());
        }
    }
}