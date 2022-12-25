#include "light-render-system.hpp"
#include "../model-system.hpp"
#include "core/engine.hpp"
#include "opaque-render-system.hpp"
namespace engine::render::_light_detail
{
    using namespace core::math;
    void LightRenderSystem::OnInstancesUpdated(core::Scene *scene)
    {
        auto &registry = scene->registry;
        std::vector<entt::entity> point_light_entities;
        for (auto const &entity : registry.view<components::TransformComponent, components::PointLight>())
        {
            point_light_entities.push_back(entity);
        }
        std::vector<entt::entity> spot_light_entities;
        for (auto const &entity : registry.view<components::TransformComponent, components::SpotLight>())
        {
            spot_light_entities.push_back(entity);
        }
        std::vector<entt::entity> directional_light_entities;
        for (auto const &entity : registry.view<components::DirectionalLight>())
        {
            directional_light_entities.push_back(entity);
        }
        std::ranges::sort(point_light_entities, [&registry] (entt::entity const &lhs, entt::entity const &rhs)
                          { return static_cast<uint32_t>(lhs) < static_cast<uint32_t>(rhs); });
        std::ranges::sort(spot_light_entities, [&registry] (entt::entity const &lhs, entt::entity const &rhs)
                          { return static_cast<uint32_t>(lhs) < static_cast<uint32_t>(rhs); });
        std::ranges::sort(directional_light_entities, [&registry] (entt::entity const &lhs, entt::entity const &rhs)
                          { return static_cast<uint32_t>(lhs) < static_cast<uint32_t>(rhs); });

        point_light_entities_temp_ = std::move(point_light_entities);
        spot_light_entities_temp_ = std::move(spot_light_entities);
        directional_light_entities_temp_ = std::move(directional_light_entities);
        refresh_data = true;
        ScheduleShadowMapUpdate();
    }

    mat4 CreateViewProjection(vec3 const &position, vec3 const &forward, vec3 const &up, float degrees = radians(90.0f))
    {
        static float kShadowRange = 100.0f;
        mat4 rv{ 1 };
        vec3 right = -normalize(cross(forward, up));
        rv[0] = vec4{ right, 0 };
        rv[1] = vec4{ up, 0 };
        rv[2] = vec4{ forward, 0 };
        rv[3] = vec4{ position, 1 };
        return perspective(degrees, 1.0f, kShadowRange, 0.01f) * invert_orthonormal(rv);
    }

    mat4 CreateOrthoViewProjection(vec3 const &position, vec3 const &direction, float resolution)
    {
        vec3 up = vec3(0.0f, 1.0f, 0.0f);
        vec3 right = normalize(cross(direction, up));
        up = normalize(cross(right, direction));
        mat4 view = look_at(position, position + direction, up);
        mat4 projection = ortho(-resolution, resolution, -resolution, resolution, 10000.0f, 0.0f);
        return projection * view;
    }

    void LightRenderSystem::ProcessPointLights(core::Scene *scene)
    {
        auto &registry = scene->registry;
        point_light_shadow_matrices_.clear();
        std::vector<_opaque_detail::OpaquePerDepthCubemap> opaque_per_cubemap_;
        uint32_t i = 0;
        for (auto const &entity : point_light_entities_)
        {
            auto const &transform = registry.get<components::TransformComponent>(entity);
            _opaque_detail::OpaquePerDepthCubemap cubemap;
            auto &g_view_projection = cubemap.g_view_projection;
            cubemap.g_slice_offset = i;
            i += 6;
            g_view_projection[0] = CreateViewProjection(transform.position, vec3(1, 0, 0), vec3(0, 1, 0));
            g_view_projection[1] = CreateViewProjection(transform.position, vec3(-1, 0, 0), vec3(0, 1, 0));
            g_view_projection[2] = CreateViewProjection(transform.position, vec3(0, 1, 0), vec3(0, 0, -1));
            g_view_projection[3] = CreateViewProjection(transform.position, vec3(0, -1, 0), vec3(0, 0, 1));
            g_view_projection[4] = CreateViewProjection(transform.position, vec3(0, 0, 1), vec3(0, 1, 0));
            g_view_projection[5] = CreateViewProjection(transform.position, vec3(0, 0, -1), vec3(0, 1, 0));
            opaque_per_cubemap_.push_back(std::move(cubemap));
            point_light_shadow_matrices_[entity] = g_view_projection;
        }
        if (opaque_per_cubemap_.size() != 0)
        {
            direct3d::api().devcon4->OMSetRenderTargets(0, nullptr, point_light_shadow_maps_.depth_stencil().depth_stencil_view());
            direct3d::api().devcon4->ClearDepthStencilView(point_light_shadow_maps_.depth_stencil().depth_stencil_view(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
            scene->renderer->opaque_render_system().RenderDepthOnly(opaque_per_cubemap_);
        }
    }
    void LightRenderSystem::ProcessSpotLights(core::Scene *scene)
    {
        auto &registry = scene->registry;
        std::vector<_opaque_detail::OpaquePerDepthTexture> opaque_per_texture_;
        spot_light_shadow_matrices_.clear();
        uint32_t i = 0;
        for (auto const &entity : spot_light_entities_)
        {
            auto const &transform = registry.get<components::TransformComponent>(entity);
            auto const &spot_light = registry.get<components::SpotLight>(entity);
            _opaque_detail::OpaquePerDepthTexture texture;
            vec3 forward = normalize(transform.rotation * core::math::vec3{ 0, 0, -1 });
            vec3 up = normalize(transform.rotation * core::math::vec3{ 0, 1, 0 });
            texture.g_view_projection = CreateViewProjection(transform.position, forward, up, spot_light.outer_cutoff);
            texture.g_slice_offset = i++;
            opaque_per_texture_.push_back(std::move(texture));
            spot_light_shadow_matrices_[entity] = texture.g_view_projection;
        }
        if (opaque_per_texture_.size() != 0)
        {
            direct3d::api().devcon4->OMSetRenderTargets(0, nullptr, spot_light_shadow_maps_.depth_stencil().depth_stencil_view());
            direct3d::api().devcon4->ClearDepthStencilView(spot_light_shadow_maps_.depth_stencil().depth_stencil_view(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
            scene->renderer->opaque_render_system().RenderDepthOnly(opaque_per_texture_);
        }
    }
    void LightRenderSystem::ProcessDirectionalLights(core::Scene *scene)
    {
        auto &registry = scene->registry;
        std::vector<_opaque_detail::OpaquePerDepthTexture> opaque_per_texture_;
        directional_light_shadow_matrices_.clear();
        uint32_t i = 0;
        for (auto const &entity : directional_light_entities_)
        {
            auto const &transform = registry.get<components::TransformComponent>(entity);
            _opaque_detail::OpaquePerDepthTexture texture;
            vec3 forward = normalize(transform.rotation * core::math::vec3{ 0, 1, 0 });
            vec3 camera_position = core::Engine::scene()->main_camera->position() - forward * 30;
            texture.g_view_projection = CreateOrthoViewProjection(camera_position, forward, 20.0f);
            texture.g_slice_offset = i++;
            opaque_per_texture_.push_back(std::move(texture));
            directional_light_shadow_matrices_[entity] = texture.g_view_projection;
        }
        if (opaque_per_texture_.size() != 0)
        {
            direct3d::api().devcon4->OMSetRenderTargets(0, nullptr, directional_light_shadow_maps_.depth_stencil().depth_stencil_view());
            direct3d::api().devcon4->ClearDepthStencilView(directional_light_shadow_maps_.depth_stencil().depth_stencil_view(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
            scene->renderer->opaque_render_system().RenderDepthOnly(opaque_per_texture_);
        }
    }
    void LightRenderSystem::RenderShadowMaps(core::Scene *scene)
    {
        if (refresh_data)
        {
            point_light_entities_ = std::move(point_light_entities_temp_);
            spot_light_entities_ = std::move(spot_light_entities_temp_);
            directional_light_entities_ = std::move(directional_light_entities_temp_);

            point_light_shadow_maps_.Resize(resolution_, point_light_entities_.size(), true);
            spot_light_shadow_maps_.Resize(resolution_, spot_light_entities_.size(), false);
            directional_light_shadow_maps_.Resize(resolution_, directional_light_entities_.size(), false);
            refresh_data = false;
        }
        
        D3D11_VIEWPORT viewport;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<float>(resolution_);
        viewport.Height = static_cast<float>(resolution_);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        direct3d::api().devcon4->RSSetViewports(1, &viewport);
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth, 0);
        direct3d::api().devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ProcessPointLights(scene);
        ProcessSpotLights(scene);
        ProcessDirectionalLights(scene);
        direct3d::api().devcon4->OMSetRenderTargets(0, nullptr, nullptr);
    }
}