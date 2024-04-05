#pragma once
#include "../../components/game-object.hpp"
#include "../ui-helpers.hpp"
namespace lighten::gui
{

    template<>
    inline void DrawComponent(entt::registry &, entt::entity entity, std::string &, components::GameObject& game_object)
    {
       if (ImGui::CollapsingHeader("Game Object"))
       {
           ImGui::Text("Name: ");
           game_object.name.reserve(game_object.name.size() + 1);
           ImGui::SameLine();
           ImGui::InputText("##name", game_object.name.data(), game_object.name.capacity());
           ImGui::Text("Tag: ");
           ImGui::SameLine();
           ImGui::InputText("##tag", game_object.tag.data(), game_object.tag.capacity());
           auto constexpr convert_entity_id = [](entt::entity id) -> uint32_t
           {
               return static_cast<uint32_t>(*reinterpret_cast<entt::id_type *>(&id));
           };
           ImGui::Text("Entity id: ");
           ImGui::SameLine();
           ImGui::Text("%u", convert_entity_id(entity));
           ImGui::Text("Parent entity id: ");
           ImGui::SameLine();
           ImGui::Text("%u", convert_entity_id(game_object.parent));
           ImGui::Text("Amount of children: ");
           ImGui::SameLine();
           ImGui::Text("%u", static_cast<uint32_t>(game_object.children.size()));
        }
    }
}