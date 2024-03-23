#pragma once
#include "core/engine.hpp"
#include "render/render.hpp"
namespace object_editor
{
    #if 0
    extern std::shared_ptr<lighten::core::Scene> selected_scene;
    extern entt::entity selected_entity;
    extern float selected_distance;

    extern ImGuizmo::OPERATION mCurrentGizmoOperation;
    extern ImGuizmo::MODE mCurrentGizmoMode;

    void RegisterKeyCallbacks();

    void OnGuiRender();

    void OnRender(glm::ivec2 const &window_pos, glm::ivec2 const &window_size);
    #endif
}