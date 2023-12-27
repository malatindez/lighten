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
        void FrameBegin()
        {
            mal_toolkit::Assert(renderer != nullptr, "Renderer is null");
            renderer->FrameBegin(this);
        }
        void DeferredRender(render::GBuffer const &buffer)
        {
            mal_toolkit::Assert(renderer != nullptr, "Renderer is null");
            renderer->DeferredRender(this, buffer);
        }
        void DeferredRender(render::GBuffer const &buffer, ID3D11DepthStencilView *dsv, ID3D11ShaderResourceView *depth_srv, ID3D11ShaderResourceView *normals_srv)
        {
            mal_toolkit::Assert(renderer != nullptr, "Renderer is null");
            renderer->DeferredRender(this, buffer, dsv, depth_srv, normals_srv);
        }
        void ForwardRender(render::PerFrame const &per_frame, render::GBuffer const &buffer, ID3D11DepthStencilView *dsv, ID3D11ShaderResourceView *depth_srv, ID3D11ShaderResourceView *normals_srv)
        {
            mal_toolkit::Assert(renderer != nullptr, "Renderer is null");
            renderer->ForwardRender(this, per_frame, buffer, dsv, depth_srv, normals_srv);
        }
        void Tick(float delta_time)
        {
            renderer->Tick(this, delta_time);
        }
        void Update()
        {
            renderer->Update(this);
        }
        void ScheduleInstanceUpdate()
        {
            mal_toolkit::Assert(renderer != nullptr, "Renderer is null");
            renderer->ScheduleInstanceUpdate();
        }
        inline void AddOpaqueInstance(uint64_t model_id, entt::entity entity)
        {
            mal_toolkit::Assert(renderer != nullptr, "Renderer is null");
            renderer->AddOpaqueInstance(model_id, registry, entity);
        }
        inline void AddOpaqueInstance(uint64_t model_id, entt::entity entity, std::vector<render::OpaqueMaterial> const &materials)
        {
            mal_toolkit::Assert(renderer != nullptr, "Renderer is null");
            renderer->AddOpaqueInstance(model_id, registry, entity, materials);
        }
        inline void AddEmissiveInstance(uint64_t model_id, entt::entity entity)
        {
            mal_toolkit::Assert(renderer != nullptr, "Renderer is null");
            renderer->AddEmissiveInstance(model_id, registry, entity);
        }
    };
}