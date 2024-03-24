#include "scene-viewer.hpp"
namespace lighten::gui
{
    SceneViewer::SceneViewer(entt::registry &registry) : registry_{registry}, observer{registry, entt::collector.update<::lighten::components::GameObject>().group<::lighten::components::GameObject>()}
    {
        registry.on_destroy<::lighten::components::GameObject>().connect<&SceneViewer::OnDestroy>(*this);
        renaming_buffer.resize(128);
    }
    void SceneViewer::OnDestroy(entt::registry &, entt::entity entity)
    {
        if (entity == selected_entity_)
        {
            selected_entity_ = entt::null;
        }
        if (entity == currently_renaming_entity)
        {
            currently_renaming_entity = entt::null;
        }
        auto &game_object = registry_.get<::lighten::components::GameObject>(entity);
        if (game_object.parent != entt::null)
        {
            auto &parent_game_object = registry_.get<::lighten::components::GameObject>(game_object.parent);
            parent_game_object.children.erase(std::remove(parent_game_object.children.begin(), parent_game_object.children.end(), entity), parent_game_object.children.end());
        }
        for (auto child : game_object.children)
        {
            auto &child_game_object = registry_.get<::lighten::components::GameObject>(child);
            child_game_object.parent = game_object.parent;
        }
        destroy_observer.push_back(entity);
    }
    void SceneViewer::AddEntity(entt::entity parent) {
        auto entity = registry_.create();
        lighten::components::GameObject game_object{};
        if (parent != entt::null) {
            game_object.parent = parent;
            game_object.name = "New Entity";
            auto& parent_game_object = registry_.get<components::GameObject>(parent);
            parent_game_object.children.push_back(entity);
        }
        registry_.emplace<components::GameObject>(entity, std::move(game_object));
        registry_.emplace<components::Transform>(entity, components::Transform::Default());
        selected_entity_ = entity;
        currently_renaming_entity = entity;
        renaming_buffer = game_object.name;
    }
    void SceneViewer::RemoveEntity(entt::entity entity) {
        // This assumes OnDestroy handles cleaning up relationships and other components as needed
        registry_.destroy(entity);
    }
    void SceneViewer::Render()
    {
        if (!destroy_observer.empty())
        {
            std::sort(
                destroy_observer.begin(),
                destroy_observer.end(),
                std::greater<entt::entity>());
            for (int i = 0; i < destroy_observer.size(); i++)
            {
                root_entities.erase(std::remove_if(root_entities.begin(), root_entities.end(),
                                                   [this](const entt::entity value)
                                                   {
                                                       // Check if value is in elemsToRemove
                                                       return std::binary_search(destroy_observer.begin(), destroy_observer.end(), value);
                                                   }),
                                    root_entities.end());
            }
            destroy_observer.clear();
        }
        if (observer.size() > 0)
        {
            observer.each([this](auto entity)
                          {
					auto& game_object = registry_.get<components::GameObject>(entity);
					if (game_object.parent == entt::null)
					{
						root_entities.push_back(entity);
					}
                    else
                    {
                        destroy_observer.push_back(entity);
                    } });
            observer.clear();
        }
        ShowEntityViewer();
        ShowInspector();
    }

    void SceneViewer::ShowEntityViewer()
    {
        if (ImGui::Begin("Entity Viewer"))
        {
            for (auto entity : root_entities)
            {
                auto entity_id = std::to_string(static_cast<uint32_t>(entity));
                DrawEntityTree(entity, entity_id);
            }
            EntityViewerContextMenu();
        }
        ImGui::End();
    }

    void SceneViewer::ShowInspector()
    {
        if (ImGui::Begin("Inspector"))
        {
            if (selected_entity_ != entt::null)
            {
                std::string entity_id = std::to_string(static_cast<uint32_t>(selected_entity_));
                mal_toolkit::constexpr_for<0, reflection::ComponentTuple::amount, 1>([this, &entity_id](auto index) constexpr -> void {
					using ComponentType = reflection::ComponentTuple::type_at<index>::type;
					if (auto* component = registry_.try_get<ComponentType>(selected_entity_); component) {
						DrawComponent(registry_, selected_entity_, entity_id, *component);
					} 
                });
            }
            else
            {
                ImGui::AlignTextToFramePadding();
                ImGui::TextDisabled("No entity selected.");
            }
        }
        ImGui::End();
    }

    void SceneViewer::EntityViewerContextMenu(entt::entity parent, const std::string_view name)
    {
        // context menu
        if (!ImGui::BeginPopupContextItem())
        {
            return;
        }

        if (ImGui::MenuItem("Add Child Entity"))
        {
            AddEntity(parent);
        }

        if (parent != entt::null)
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
        auto &game_object = registry_.get<components::GameObject>(entity);
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
                        auto &dragged_obj = registry_.get<components::GameObject>(dragged_entity);

                        auto *dragged_entity_old_parent_obj = dragged_obj.parent == entt::null ? nullptr : registry_.try_get<components::GameObject>(dragged_obj.parent);

                        if (auto *dragged_world_transform = registry_.try_get<components::WorldTransform>(dragged_entity);
                            dragged_world_transform)
                        {
                            auto* new_parent_transform = registry_.try_get<components::WorldTransform>(entity);

                            // local -> world -> new_local
                            glm::mat4 conversion_matrix = (new_parent_transform ? new_parent_transform->inv_world : glm::mat4{ 1.0f }) * dragged_world_transform->world;

                            registry_.patch<::lighten::components::Transform>(dragged_entity, [&conversion_matrix](auto &transform)
                                                                            {
                                    transform.position = glm::vec3(conversion_matrix[3]);
                                    transform.rotation = glm::quat_cast(conversion_matrix);
                                    transform.scale = glm::vec3(glm::length(glm::vec3(conversion_matrix[0])), 
                                                                glm::length(glm::vec3(conversion_matrix[1])), 
                                                                glm::length(glm::vec3(conversion_matrix[2]))); });
                        }
                        // finish up..

                        if (dragged_entity_old_parent_obj)
                        {
                            auto &children = dragged_entity_old_parent_obj->children;
                            children.erase(std::remove(children.begin(), children.end(), dragged_entity), children.end());
                        }

                        dragged_obj.parent = entity;
                        game_object.children.push_back(dragged_entity);

                        registry_.patch<::lighten::components::GameObject>(dragged_entity, [](auto &) {});
                        registry_.patch<::lighten::components::GameObject>(entity, [](auto &) {});
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