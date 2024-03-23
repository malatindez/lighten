#pragma once
#include "components/gui-components.hpp"
#include "../reflection/reflection.hpp"
#include "mal-toolkit/template.hpp"
namespace lighten::gui
{
    struct SceneViewer
    {
    public:
        SceneViewer(entt::registry &registry) : registry_{registry} {}

        void ShowEntitiesViewer() {
            if (ImGui::Begin("Entities Viewer")) {
                registry_.view<components::GameObject>().each([this](auto entity, auto& game_object) {
                    if(game_object.parent == entt::null)
                    {
                        auto entity_id = std::to_string(static_cast<uint32_t>(entity));
                        DrawEntityTree(registry_, entity, entity_id);
                    }
                });
            }
            ImGui::End();
        }
    private:
        void DrawEntityTree(entt::entity entity, std::string& entity_id) {
            auto& game_object = registry_.get<::lighten::components::GameObject>(entity);

            if (ImGui::TreeNode(game_object.name.c_str())) {
                mal_toolkit::constexpr_for<0, reflection::ComponentTuple::amount, 1>([this](auto index) constexpr -> void {
                    using ComponentType = reflection::ComponentTuple::type_at<index>::type;
                    if (auto* component = registry_.try_get<ComponentType>(entity); component) {
                        DrawComponent(registry_, entity, entity_id, *component);
                    }
                });
                // Recursively draw children
                for (auto child : game_object.children) {
                    auto child_id = std::to_string(static_cast<uint32_t>(child));
                    DrawEntityTree(registry_, child, child_id);
                }

                ImGui::TreePop();
            }
        }
        entt::registry &registry_;
    };

}