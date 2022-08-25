#pragma once
#include "renderer.hpp"
#include "misc/camera-controller.hpp"
class Controller
    : public engine::core::Layer,
    public engine::core::Layer::HandleTick,
    public engine::core::Layer::HandleEvent,
    public engine::core::Layer::HandleGuiRender
{
public:
    using UpdateCallback = std::function<void(float)>;
    using engine::core::Layer::Layer;
    Controller(std::shared_ptr<Renderer> renderer, engine::core::math::ivec2 const &window_size);
    std::vector<std::function<void(float)>> &update_callbacks() { return update_callbacks_; }
    void OnTick(float delta_time) override;
    void OnEvent(engine::core::events::Event &e) override;

    void OnGuiRender() override
    {
        ImGui::Begin("Framerate");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Projection matrix: ");
        ImGui::Text(engine::utils::FormatToString(first_scene->main_camera->camera().projection).c_str());
        ImGui::Text("InvProjection matrix: ");
        ImGui::Text(engine::utils::FormatToString(first_scene->main_camera->camera().inv_projection).c_str());
        ImGui::Text("View matrix: ");
        ImGui::Text(engine::utils::FormatToString(first_scene->main_camera->camera().view).c_str());
        ImGui::Text("InvView matrix: ");
        ImGui::Text(engine::utils::FormatToString(first_scene->main_camera->camera().inv_view).c_str());
        ImGui::Text("ViewProjection matrix: ");
        ImGui::Text(engine::utils::FormatToString(first_scene->main_camera->camera().view_projection).c_str());
        ImGui::Text("InvViewProjection matrix: ");
        ImGui::Text(engine::utils::FormatToString(first_scene->main_camera->camera().inv_view_projection).c_str());
        ImGui::Text("Inverse(InvViewProjection) matrix: ");
        ImGui::Text(engine::utils::FormatToString(inverse(first_scene->main_camera->camera().inv_view_projection)).c_str());
        ImGui::Text("ViewProjection - Inverse(InvViewProjection) matrix: ");
        ImGui::Text(engine::utils::FormatToString(first_scene->main_camera->camera().view_projection - inverse(first_scene->main_camera->camera().inv_view_projection)).c_str());
        ImGui::Text("Position: ");
        ImGui::Text(engine::utils::FormatToString(first_scene->main_camera->transform().position).c_str());
        ImGui::End();
    }

private:
    engine::core::math::ivec2 const &current_mouse_position_;

private:
    engine::core::math::ivec2 previous_mouse_position{ 0, 0 };

    std::shared_ptr<engine::core::Scene> first_scene;

private:
    std::vector<UpdateCallback> update_callbacks_;
    std::shared_ptr<Renderer> renderer_;
};