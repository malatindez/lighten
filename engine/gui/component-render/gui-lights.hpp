#pragma once
#include "../../components/lights.hpp"
#include "../gui/ui-helpers.hpp"
namespace lighten::gui
{
    template<>
    void DrawComponent<components::DirectionalLight>(entt::registry &registry, entt::entity entity, std::string &entity_id, components::DirectionalLight& light)
    {
        if (ImGui::CollapsingHeader(("Directional light##" + entity_id).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::ColorEdit3(("Color##" + entity_id).c_str(), reinterpret_cast<float *>(&directional_light->color));
            ImGui::SliderFloat(("Power##" + entity_id).c_str(), &directional_light->power, 0, 1e6f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::InputFloat(("Solid angle##" + entity_id).c_str(), &directional_light->solid_angle);
        }
    }

    template<>
    void DrawComponent<components::PointLight>(entt::registry &registry, entt::entity entity, std::string &entity_id, components::PointLight& light)
    {
          if (ImGui::CollapsingHeader(("Point light##" + entity_id).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::ColorEdit3(("Color##" + entity_id).c_str(), reinterpret_cast<float *>(&point_light->color));
            ImGui::SliderFloat(("Power##" + entity_id).c_str(), &point_light->power, 0, 1e6f, "%.3f", ImGuiSliderFlags_Logarithmic);
        }
    }

    template<>
    void DrawComponent<components::SpotLight>(entt::registry &registry, entt::entity entity, std::string &entity_id, components::SpotLight& light)
    {
        if (ImGui::CollapsingHeader(("Spot light##" + entity_id).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::ColorEdit3(("Color##" + entity_id).c_str(), reinterpret_cast<float *>(&spot_light->color));
            ImGui::SliderFloat(("Power##" + entity_id).c_str(), &spot_light->power, 0, 1e6f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderAngle(("Inner cutoff##" + entity_id).c_str(), &spot_light->inner_cutoff, 0, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderAngle(("Outer cutoff##" + entity_id).c_str(), &spot_light->outer_cutoff, 0, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
            if (spot_light->inner_cutoff >= spot_light->outer_cutoff)
            {
                spot_light->inner_cutoff = spot_light->outer_cutoff * 0.95f;
            }
        }
    }
}