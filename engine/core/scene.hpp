#pragma once
#include "misc/camera-controller.hpp"
#include "render/renderer.hpp"
namespace engine::core
{
    class Scene
    {
    public:
        entt::registry registry;
        std::unique_ptr<render::Renderer> renderer = nullptr;
        std::unique_ptr<engine::CameraController> main_camera = nullptr;
        void Render(render::PerFrame const &per_frame)
        {
            utils::Assert(renderer != nullptr, "Renderer is null");
            renderer->Render(this, per_frame);
        }
        void Tick(float delta_time)
        {
            renderer->Tick(this, delta_time);
        }
        void Update()
        {
            renderer->Update(this);
        }
        void ScheduleOnInstancesUpdate()
        {
            utils::Assert(renderer != nullptr, "Renderer is null");
            renderer->ScheduleOnInstancesUpdate();
        }
        inline void AddOpaqueInstance(uint64_t model_id, entt::entity entity)
        {
            utils::Assert(renderer != nullptr, "Renderer is null");
            renderer->AddOpaqueInstance(model_id, registry, entity);
        }
        inline void AddOpaqueInstance(uint64_t model_id, entt::entity entity, std::vector<render::OpaqueMaterial> const &materials)
        {
            utils::Assert(renderer != nullptr, "Renderer is null");
            renderer->AddOpaqueInstance(model_id, registry, entity, materials);
        }
        inline void AddEmissiveInstance(uint64_t model_id, entt::entity entity)
        {
            utils::Assert(renderer != nullptr, "Renderer is null");
            renderer->AddEmissiveInstance(model_id, registry, entity);
        }
        inline void AddEmissiveInstance(uint64_t model_id, entt::entity entity, std::vector<render::EmissiveMaterial> const &materials)
        {
            utils::Assert(renderer != nullptr, "Renderer is null");
            renderer->AddEmissiveInstance(model_id, registry, entity, materials);
        }
    };
}