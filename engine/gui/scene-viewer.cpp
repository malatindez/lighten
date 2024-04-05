#include "scene-viewer.hpp"
namespace lighten::gui
{
    SceneViewer::SceneViewer(std::shared_ptr<core::World> world) : 
            world_{world}
    {
        renaming_buffer.resize(128);
    }
    void SceneViewer::AddEntity(entt::entity parent) {
        auto entity = world_->CreateGameObject("New Entity", parent);
        selected_entity_ = entity;
        currently_renaming_entity = entity;
        renaming_buffer = "New Entity";
    }
    void SceneViewer::RemoveEntity(entt::entity entity) {
        world_->registry().destroy(entity);
    }
    void SceneViewer::Render()
    {
        ShowEntityViewer();
        ShowInspector();
    }

    void SceneViewer::ShowEntityViewer()
    {
        if (ImGui::Begin("Entity Viewer"))
        {
            entt::entity entity = world_->world_scene();
            auto entity_id = std::to_string(static_cast<uint32_t>(entity));
            DrawEntityTree(entity, entity_id);
            EntityViewerContextMenu();
        }
        ImGui::End();
    }

    void SceneViewer::ShowInspector()
    {
        if (ImGui::Begin("Inspector"))
        {
            bool is_null = selected_entity_ == entt::null;
            if (!is_null && !world_->registry().valid(selected_entity_))
            {
                selected_entity_ = entt::null;
                is_null = true;
            }
            if (is_null)
            {
                ImGui::AlignTextToFramePadding();
                ImGui::TextDisabled("No entity selected.");
                return;
            }
            std::string entity_id = std::to_string(static_cast<uint32_t>(selected_entity_));
            mal_toolkit::constexpr_for<0, reflection::ComponentTuple::amount, 1>([this, &entity_id](auto index) constexpr -> void {
            	using ComponentType = reflection::ComponentTuple::type_at<index>::type;
            	if (ComponentType* component = world_->registry().try_get<ComponentType>(selected_entity_); 
                    component) {
            		DrawComponent(world_->registry(), selected_entity_, entity_id, *component);
            	} 
            });
        }
        ImGui::End();
    }

    void SceneViewer::EntityViewerContextMenu(entt::entity parent, const std::string_view name)
    {
        // context menu 
        bool is_orphan = parent == entt::null;
        static const char* parented_str = "Add child entity";
        static const char* orphan_str = "Create new entity";
        if (ImGui::MenuItem(is_orphan ? orphan_str : parented_str))
        {
            AddEntity(parent);
        }
        
        if (!is_orphan)
        {
            ImGui::Separator();
            if (ImGui::MenuItem("Rename"))
            {
                currently_renaming_entity = parent;
                renaming_buffer = name;
            }
        }

        // TODO
//        if (ImGui::MenuItemDisabled("Duplicate"))
//        {
//            DuplicateEntity(entity);
//        }

        ImGui::Separator();
        if (ImGui::MenuItem("Remove Entity"))
        {
            RemoveEntity(parent);
            ImGui::EndPopup();
            return; // Early return to avoid accessing destroyed entity
        }
        ImGui::EndPopup();
        
    }

    void SceneViewer::DrawEntityTree(entt::entity entity, std::string &entity_id)
    {
        auto &registry = world_->registry();
        auto &game_object = registry.get<components::GameObject>(entity);
        bool isRenaming = (currently_renaming_entity == entity);
        std::string node_label = game_object.name + "##" + entity_id;
        if (isRenaming)
        {
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText((node_label + "###rename").c_str(), renaming_buffer.data(), renaming_buffer.capacity(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

            if (!ImGui::IsItemFocused() ||
                ImGui::IsItemDeactivatedAfterEdit() ||
                ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter), false) ||
                ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_KeyPadEnter), false))
            {
                std::cout << "a";
                game_object.name = renaming_buffer;
                spdlog::info(game_object.name);
                currently_renaming_entity = entt::null;
            }
            return;
        }
        bool leaf = game_object.children.empty();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
        flags |= selected_entity_ == entity ? ImGuiTreeNodeFlags_Selected : 0;
        flags |= leaf ? ImGuiTreeNodeFlags_Leaf : 0;
        bool node_opened = ImGui::TreeNodeEx(node_label.c_str(), flags, "%s", game_object.name.c_str());

        // Handling selection and renaming
        {
            if (ImGui::IsItemClicked())
            {
                selected_entity_ = entity;
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    currently_renaming_entity = entity;
                    renaming_buffer = game_object.name; // Initialize buffer with the current name
                    ImGui::SetKeyboardFocusHere(-1);    // Focus on the input text in the next frame
                }
            }
        }

        // Dragging
        {
            // Drag source: marking the current node as draggable
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("SCENE_VIEWER_TREE_NODE", &entity, sizeof(entt::entity));
                ImGui::Text("Move %s", game_object.name.c_str());
                ImGui::EndDragDropSource();
            }

            // Drop target: handling dropping on this node
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCENE_VIEWER_TREE_NODE"))
                    {
                        assert(payload->DataSize == sizeof(entt::entity));

                        entt::entity dragged_entity = *static_cast<const entt::entity *>(payload->Data);
                        auto &dragged_obj = registry.get<components::GameObject>(dragged_entity);

                        auto *dragged_entity_old_parent_obj = dragged_obj.parent == entt::null ? nullptr : registry.try_get<components::GameObject>(dragged_obj.parent);

                        if (auto *dragged_world_transform = registry.try_get<components::WorldTransform>(dragged_entity);
                            dragged_world_transform)
                        {
                            auto* new_parent_transform = registry.try_get<components::WorldTransform>(entity);

                            // local -> world -> new_local
                            glm::mat4 conversion_matrix = (new_parent_transform ? new_parent_transform->inv_world : glm::mat4{ 1.0f }) * dragged_world_transform->world;

                            registry.patch<::lighten::components::Transform>(dragged_entity, [&conversion_matrix](auto &transform)
                                                                            {
                                    transform.position = glm::vec3(conversion_matrix[3]);
                                    transform.rotation = glm::quat_cast(conversion_matrix);
                                    transform.scale = glm::vec3(glm::length(glm::vec3(conversion_matrix[0])), 
                                                                glm::length(glm::vec3(conversion_matrix[1])), 
                                                                glm::length(glm::vec3(conversion_matrix[2]))); });
                        }
                        
                        if (dragged_entity_old_parent_obj)
                        {
                            auto &children = dragged_entity_old_parent_obj->children;
                            children.erase(std::remove(children.begin(), children.end(), dragged_entity), children.end());
                        }

                        dragged_obj.parent = entity;
                        game_object.children.push_back(dragged_entity);

                        registry.patch<::lighten::components::GameObject>(dragged_entity, [](auto &) {});
                        registry.patch<::lighten::components::GameObject>(entity, [](auto &) {});
                    }
                ImGui::EndDragDropTarget();
            }
        }
        EntityViewerContextMenu(entity, game_object.name);

        if (node_opened)
        {
            // Recursively draw children
            for (auto child : game_object.children)
            {
                auto child_id = std::to_string(static_cast<uint32_t>(child));
                DrawEntityTree(child, child_id);
            }
            ImGui::TreePop();
        }
    }
}