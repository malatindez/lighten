#pragma once
#include "components/gui-components.hpp"
#include "../reflection/reflection.hpp"
#include "mal-toolkit/template.hpp"
namespace lighten::gui
{
    struct SceneViewer
    {
    public:
        SceneViewer(std::shared_ptr<core::World> world);
        void Render();
        inline entt::entity &selected_entity() { return selected_entity_; }

    private:

        void ShowEntityViewer();
        void ShowInspector();
        void EntityViewerContextMenu(entt::entity parent = entt::null, const std::string_view name = "");
        
        
        void DrawEntityTree(entt::entity entity, std::string &entity_id);
        
        void AddEntity(entt::entity parent = entt::null);
        void RemoveEntity(entt::entity entity);
        
        std::shared_ptr<core::World> world_;

        entt::entity selected_entity_ = entt::null;
        entt::entity currently_renaming_entity = entt::null;
        
        std::string renaming_buffer = "";
    };

}