#pragma once
#include "misc/camera-controller.hpp"
#include "direct3d11/deferred-hdr-render-pipeline.hpp"
#include "core/engine.hpp"
class Controller
    : public lighten::core::Layer,
      public lighten::core::Layer::HandleTick,
      public lighten::core::Layer::HandleEvent,
      public lighten::core::Layer::HandleGuiRender,
      public lighten::core::Layer::HandleUpdate
{
public:
    using UpdateCallback = std::function<void(float)>;
    using lighten::core::Layer::Layer;
    entt::entity main_camera_entity;
    entt::entity last_created_knight;

    Controller(std::shared_ptr<lighten::direct3d::DeferredHDRRenderPipeline> hdr_render_pipeline);
    std::vector<std::function<void(float)>> &update_callbacks() { return update_callbacks_; }
    void OnTick(float delta_time) override;
    void OnEvent(lighten::core::events::Event &e) override;
    void OnGuiRender() override;
    void OnUpdate() override;
    uint32_t current_view_proj = std::numeric_limits<uint32_t>::max();
    glm::mat4 view_proj = glm::mat4{1.0f};

private:
    std::shared_ptr<lighten::core::Scene> first_scene;

private:
    std::shared_ptr<lighten::direct3d::DeferredHDRRenderPipeline> hdr_render_pipeline_;
    std::vector<UpdateCallback> update_callbacks_;
};