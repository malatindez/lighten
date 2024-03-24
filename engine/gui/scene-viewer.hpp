#pragma once
#include "components/gui-components.hpp"
#include "../reflection/reflection.hpp"
#include "mal-toolkit/template.hpp"
namespace lighten::gui
{
    struct SceneViewer
    {
    public:
        SceneViewer(entt::registry &registry);
        void OnDestroy(entt::registry &, entt::entity entity);

        void Render();

        void ShowEntityViewer();

        void ShowInspector();
        inline entt::entity &selected_entity() { return selected_entity_; }

    private:
        void AddEntity(entt::entity parent = entt::null);
        void RemoveEntity(entt::entity entity);
        void EntityViewerContextMenu(entt::entity parent = entt::null, const std::string_view name = "");
        
        void DrawEntityTree(entt::entity entity, std::string &entity_id);
        entt::registry &registry_;
        entt::entity selected_entity_ = entt::null;
        entt::entity currently_renaming_entity = entt::null;
        std::string renaming_buffer;

        entt::observer observer;
        std::vector<entt::entity> root_entities;
        std::vector<entt::entity> destroy_observer;
    };

}