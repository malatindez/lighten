#include "object-editor.hpp"

using namespace engine::components;

namespace scene_viewer
{
    void DrawGuiEntity(entt::registry &registry, entt::entity entity, std::vector<entt::entity> &drawn, bool subnode = false)
    {
        drawn.push_back(entity);
        auto game_object = registry.try_get<GameObject>(entity);
        std::string name = "None";
        if (game_object != nullptr)
        {
            if (game_object->parent != entt::null && !subnode)
            {
                return;
            }
            name = game_object->name;
        }
        else
        {
            game_object = &registry.emplace<GameObject>(entity);
            game_object->name = name;
        }

        if (ImGui::TreeNode((name + " ##" + std::to_string(*reinterpret_cast<entt::id_type *>(&entity))).c_str()))
        {
            for (auto &child : game_object->children)
            {
                DrawGuiEntity(registry, child, drawn, true);
            }
            if (ImGui::Button("Add child Entity"))
            {
                auto new_entity = registry.create();
                auto &t = registry.emplace<GameObject>(new_entity);
                t.name = "New Entity";
                game_object->children.push_back(new_entity);
                t.parent = entity;
            }
            ImGui::SameLine();
            if (ImGui::Button("Select"))
            {
                object_editor::selected_entity = entity;
                object_editor::selected_scene = engine::core::Engine::scene();
            }
            ImGui::TreePop();
        }
    }
    void OnGuiRender()
    {
        ImGui::Begin("Scene Viewer");
        // get scene
        auto scene = engine::core::Engine::scene();
        auto &registry = scene->registry;
        // draw entity tree
        ImGui::BeginChild("##entity_tree", ImVec2(0, ImGui::GetWindowHeight() * 0.8f), true);
        std::vector<entt::entity> drawn;
        registry.each([&drawn, &registry] (auto entity) {
            if (std::find(drawn.begin(), drawn.end(), entity) != drawn.end())
                return;
            DrawGuiEntity(registry, entity, drawn);
                      });
        ImGui::EndChild();
        if (ImGui::Button("Add Entity"))
        {
            auto entity = registry.create();
            registry.emplace<GameObject>(entity).name = "New Entity";
        }
        ImGui::End();
    }
}