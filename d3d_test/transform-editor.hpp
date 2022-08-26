#pragma once
#include "core/engine.hpp"
#include "render/render.hpp"
namespace transform_editor
{

    extern std::shared_ptr<engine::core::Scene> selected_scene;
    extern entt::entity selected_entity;
    extern float selected_distance;

    extern ImGuizmo::OPERATION mCurrentGizmoOperation;
    extern ImGuizmo::MODE mCurrentGizmoMode;


    void RegisterKeyCallbacks();

    void OnGuiRender();
}