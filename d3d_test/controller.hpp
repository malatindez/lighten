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
    entt::entity main_camera_entity;
    entt::entity last_created_knight;

    Controller(std::shared_ptr<Renderer> renderer, engine::core::math::ivec2 const &window_size);
    std::vector<std::function<void(float)>> &update_callbacks() { return update_callbacks_; }
    void OnTick(float delta_time) override;
    void OnEvent(engine::core::events::Event &e) override;
    void OnGuiRender() override
    {
        ImGui::Begin("Framerate");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Move speed: ");
        ImGui::Text("%.3f", first_scene->main_camera->move_speed());
        ImGui::Text("FOV: ");
        ImGui::Text("%.3f", first_scene->main_camera->fovy());
        ImGui::Text("Flags: ");
        ImGui::Text("%u", first_scene->main_camera->flags());
        ImGui::Text("Position: ");
        auto &temp1 = first_scene->main_camera->transform();
        auto &temp2 = first_scene->registry.get<engine::components::TransformComponent>(first_scene->main_camera->camera_);
        ImGui::Text("%s", engine::utils::FormatToString(first_scene->main_camera->transform().position).c_str());
        ImGui::End();
    }

private:
    engine::core::math::ivec2 const &current_mouse_position_;

private:
    engine::core::math::ivec2 previous_mouse_position{ 0, 0 };
    std::shared_ptr<engine::core::Scene> first_scene;

    std::shared_ptr<engine::core::Scene> selected_entity_scene_ = nullptr;
    entt::entity selected_entity_ = entt::null;

    engine::core::math::ivec2 rb_saved_mouse_position_{ 0 };
    float selected_object_distance_ = 0.0f;
    engine::core::math::vec3 selected_object_offset_{ 0.0f };

private:
    std::vector<UpdateCallback> update_callbacks_;
    std::shared_ptr<Renderer> renderer_;
};