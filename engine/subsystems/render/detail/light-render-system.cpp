#include "light-render-system.hpp"
#include "../model-system.hpp"
#include "core/engine.hpp"
#include "opaque-render-system.hpp"
namespace engine::render::_light_detail
{
    using namespace core::math;
    void LightRenderSystem::UpdateInstances(core::Scene *scene)
    {
        auto &registry = scene->registry;
        std::vector<entt::entity> point_light_entities;
        for (auto const &entity : registry.view<components::Transform, components::PointLight>())
        {
            point_light_entities.push_back(entity);
        }
        std::vector<entt::entity> spot_light_entities;
        for (auto const &entity : registry.view<components::Transform, components::SpotLight>())
        {
            spot_light_entities.push_back(entity);
        }
        std::vector<entt::entity> directional_light_entities;
        for (auto const &entity : registry.view<components::DirectionalLight>())
        {
            directional_light_entities.push_back(entity);
        }
        std::ranges::sort(point_light_entities, [] (entt::entity const &lhs, entt::entity const &rhs)
                          { return static_cast<uint32_t>(lhs) < static_cast<uint32_t>(rhs); });
        std::ranges::sort(spot_light_entities, [] (entt::entity const &lhs, entt::entity const &rhs)
                          { return static_cast<uint32_t>(lhs) < static_cast<uint32_t>(rhs); });
        std::ranges::sort(directional_light_entities, [] (entt::entity const &lhs, entt::entity const &rhs)
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
            auto const &point_light = registry.get<components::PointLight>(entity);
            if (point_light.casts_shadows == false)
                continue;
            auto const &transform = registry.get<components::Transform>(entity);
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
            scene->renderer->opaque_render_system().RenderDepthOnly(opaque_per_cubemap_, scene);
            scene->renderer->dissolution_render_system().RenderDepthOnly(opaque_per_cubemap_, scene);
            scene->renderer->grass_render_system().RenderDepthOnly(opaque_per_cubemap_, scene);
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
            auto const &spot_light = registry.get<components::SpotLight>(entity);
            if (spot_light.casts_shadows == false)
                continue;
            auto const &transform = registry.get<components::Transform>(entity);
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
            scene->renderer->opaque_render_system().RenderDepthOnly(opaque_per_texture_, scene);
            scene->renderer->dissolution_render_system().RenderDepthOnly(opaque_per_texture_, scene);
            scene->renderer->grass_render_system().RenderDepthOnly(opaque_per_texture_, scene);
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
            auto const &directional_light = registry.get<components::DirectionalLight>(entity);
            if (directional_light.casts_shadows == false)
                continue;
            auto const &transform = registry.get<components::Transform>(entity);
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
            scene->renderer->opaque_render_system().RenderDepthOnly(opaque_per_texture_, scene);
            scene->renderer->dissolution_render_system().RenderDepthOnly(opaque_per_texture_, scene);
            scene->renderer->grass_render_system().RenderDepthOnly(opaque_per_texture_, scene);
        }
    }
    void LightRenderSystem::OnRender(core::Scene *scene)
    {
        if (is_instance_update_scheduled_)
        {
            UpdateInstances(scene);
            is_instance_update_scheduled_ = false;
        }
        bool should_return = false;
        if (should_update && shadow_map_update_timer_.elapsed() > shadow_map_update_interval_)
        {
            shadow_map_update_timer_.reset();
            should_update = false;
        }
        else
        {
            should_return = true;
        }

        if (refresh_data)
        {
            auto &registry = scene->registry;
            point_light_entities_ = std::move(point_light_entities_temp_);
            spot_light_entities_ = std::move(spot_light_entities_temp_);
            directional_light_entities_ = std::move(directional_light_entities_temp_);
            uint32_t point_shadow_lights = 0;
            uint32_t spot_shadow_lights = 0;
            uint32_t directional_shadow_lights = 0;
            // for some reason returns 1 less than it should
            // TODO (look into it):
            //std::accumulate(point_light_entities_.begin(), point_light_entities_.end(), 0u,
            //                                         [&registry] (uint32_t value, entt::entity const &entity)
            //                                        {
            //                                           return value + registry.get<components::PointLight>(entity).casts_shadows ? 1u : 0u;
            //                                      });
            for (auto &entity : point_light_entities_)
            {
                point_shadow_lights += registry.get<components::PointLight>(entity).casts_shadows ? 1u : 0u;
            }
            for (auto &entity : spot_light_entities_)
            {
                spot_shadow_lights += registry.get<components::SpotLight>(entity).casts_shadows ? 1u : 0u;
            }
            for (auto &entity : directional_light_entities_)
            {
                directional_shadow_lights += registry.get<components::DirectionalLight>(entity).casts_shadows ? 1u : 0u;
            }

            point_light_shadow_maps_.Resize(resolution_, point_shadow_lights, true);
            spot_light_shadow_maps_.Resize(resolution_, spot_shadow_lights, false);
            directional_light_shadow_maps_.Resize(resolution_, directional_shadow_lights, false);
            refresh_data = false;
        }
        if (!should_return)
        {
            D3D11_VIEWPORT viewport;
            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            viewport.Width = static_cast<float>(resolution_);
            viewport.Height = static_cast<float>(resolution_);
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            UINT viewport_count = 1;
            D3D11_VIEWPORT saved_viewport;
            direct3d::api().devcon4->RSGetViewports(&viewport_count, &saved_viewport);

            direct3d::api().devcon4->RSSetViewports(viewport_count, &viewport);
            direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth, 0);
            direct3d::api().devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            ProcessPointLights(scene);
            ProcessSpotLights(scene);
            ProcessDirectionalLights(scene);
            direct3d::api().devcon4->RSSetViewports(viewport_count, &saved_viewport);
        }
        auto &registry = scene->registry;
        {
            auto const &point_lights = point_light_entities();
            auto const &spot_lights = spot_light_entities();
            auto const &directional_lights = directional_light_entities();
            auto const &point_light_matrices = point_light_shadow_matrices();
            auto const &spot_light_matrices = spot_light_shadow_matrices();
            auto const &directional_light_matrices = directional_light_shadow_matrices();
            per_frame.num_point_lights = per_frame.num_spot_lights = per_frame.num_directional_lights = 0;
            per_frame.shadow_num_point_lights = per_frame.shadow_num_spot_lights = per_frame.shadow_num_directional_lights = 0;
            point_light_shadow_map_indices_.clear();
            spot_light_shadow_map_indices_.clear();
            directional_light_shadow_map_indices_.clear();
            for (entt::entity entity : point_lights)
            {
                auto &registry_point_light = registry.get<components::PointLight>(entity);
                auto &registry_transform = registry.get<components::Transform>(entity);
                if (registry_point_light.casts_shadows == false)
                {
                    auto &opaque_point_light = per_frame.point_lights[per_frame.num_point_lights];
                    opaque_point_light.color = registry_point_light.color * registry_point_light.power;
                    opaque_point_light.position = registry_transform.position;
                    opaque_point_light.radius = length(registry_transform.scale) / sqrt(3.1f);
                    if (++per_frame.num_point_lights >= LightsPerFrame::kMaxPointLights)
                    {
                        mal_toolkit::AlwaysAssert(false, "Amount of point lights on the scene went beyond the maximum amount.");
                        break;
                    }
                }
                else
                {
                    if (!point_light_matrices.contains(entity))
                    {
                        continue;
                    }
                    auto &opaque_point_light = per_frame.shadow_point_lights[per_frame.shadow_num_point_lights];
                    opaque_point_light.color = registry_point_light.color * registry_point_light.power;
                    opaque_point_light.position = registry_transform.position;
                    opaque_point_light.radius = length(registry_transform.scale) / sqrt(3.1f);
                    opaque_point_light.view_projection = point_light_matrices.at(entity);
                    point_light_shadow_map_indices_[entity] = per_frame.shadow_num_point_lights;
                    if (++per_frame.shadow_num_point_lights >= LightsPerFrame::kMaxPointLights)
                    {
                        mal_toolkit::AlwaysAssert(false, "Amount of point lights on the scene went beyond the maximum amount.");
                        break;
                    }
                }
            }
            for (entt::entity entity : spot_lights)
            {
                auto &registry_spot_light = registry.get<components::SpotLight>(entity);
                auto &registry_transform = registry.get<components::Transform>(entity);
                if (registry_spot_light.casts_shadows == false)
                {
                    auto &opaque_spot_light = per_frame.spot_lights[per_frame.num_spot_lights];
                    opaque_spot_light.color = registry_spot_light.color * registry_spot_light.power;
                    opaque_spot_light.position = registry_transform.position;
                    opaque_spot_light.direction = registry_transform.rotation * core::math::vec3(0, 0, 1);
                    opaque_spot_light.radius = length(registry_transform.scale) / sqrt(3.1f);
                    opaque_spot_light.inner_cutoff = registry_spot_light.inner_cutoff;
                    opaque_spot_light.outer_cutoff = registry_spot_light.outer_cutoff;
                    if (++per_frame.num_spot_lights >= LightsPerFrame::kMaxSpotLights)
                    {
                        mal_toolkit::AlwaysAssert(false, "Amount of spot lights on the scene went beyond the maximum amount.");
                        break;
                    }
                }
                else
                {
                    if (!spot_light_matrices.contains(entity))
                    {
                        continue;
                    }
                    auto &opaque_spot_light = per_frame.shadow_spot_lights[per_frame.shadow_num_spot_lights];
                    opaque_spot_light.color = registry_spot_light.color * registry_spot_light.power;
                    opaque_spot_light.position = registry_transform.position;
                    opaque_spot_light.direction = registry_transform.rotation * core::math::vec3(0, 0, 1);
                    opaque_spot_light.radius = length(registry_transform.scale) / sqrt(3.1f);
                    opaque_spot_light.inner_cutoff = registry_spot_light.inner_cutoff;
                    opaque_spot_light.outer_cutoff = registry_spot_light.outer_cutoff;
                    opaque_spot_light.view_projection = spot_light_matrices.at(entity);
                    spot_light_shadow_map_indices_[entity] = per_frame.shadow_num_spot_lights;
                    if (++per_frame.shadow_num_spot_lights >= LightsPerFrame::kMaxSpotLights)
                    {
                        mal_toolkit::AlwaysAssert(false, "Amount of spot lights on the scene went beyond the maximum amount.");
                        break;
                    }
                }
            }
            for (entt::entity entity : directional_lights)
            {
                auto &registry_directional_light = registry.get<components::DirectionalLight>(entity);
                auto &registry_transform = registry.get<components::Transform>(entity);
                if (registry_directional_light.casts_shadows == false)
                {
                    auto &opaque_directional_light = per_frame.directional_lights[per_frame.num_directional_lights];
                    opaque_directional_light.color = registry_directional_light.color * registry_directional_light.power;
                    opaque_directional_light.direction = core::math::normalize(registry_transform.rotation * core::math::vec3{ 0, 1, 0 });
                    opaque_directional_light.solid_angle = registry_directional_light.solid_angle;
                    if (++per_frame.num_directional_lights >= LightsPerFrame::kMaxDirectionalLights)
                    {
                        mal_toolkit::AlwaysAssert(false, "Amount of directional lights on the scene went beyond the maximum amount.");
                        break;
                    }
                }
                else
                {
                    if (!directional_light_matrices.contains(entity))
                    {
                        continue;
                    }
                    auto &opaque_directional_light = per_frame.shadow_directional_lights[per_frame.shadow_num_directional_lights];
                    opaque_directional_light.color = registry_directional_light.color * registry_directional_light.power;
                    opaque_directional_light.direction = core::math::normalize(registry_transform.rotation * core::math::vec3{ 0, 1, 0 });
                    opaque_directional_light.solid_angle = registry_directional_light.solid_angle;
                    opaque_directional_light.view_projection = directional_light_matrices.at(entity);
                    directional_light_shadow_map_indices_[entity] = per_frame.shadow_num_directional_lights;
                    if (++per_frame.shadow_num_directional_lights >= LightsPerFrame::kMaxDirectionalLights)
                    {
                        mal_toolkit::AlwaysAssert(false, "Amount of directional lights on the scene went beyond the maximum amount.");
                        break;
                    }
                }
            }
        }
        per_frame.point_light_shadow_resolution = point_light_shadow_resolution();
        per_frame.spot_light_shadow_resolution = spot_light_shadow_resolution();
        per_frame.directional_light_shadow_resolution = directional_light_shadow_resolution();
        lights_per_frame_.Update(per_frame);
        lights_per_frame_.Bind(direct3d::ShaderType::VertexShader, 1);
        lights_per_frame_.Bind(direct3d::ShaderType::HullShader, 1);
        lights_per_frame_.Bind(direct3d::ShaderType::DomainShader, 1);
        lights_per_frame_.Bind(direct3d::ShaderType::GeometryShader, 1);
        lights_per_frame_.Bind(direct3d::ShaderType::PixelShader, 1);
        lights_per_frame_.Bind(direct3d::ShaderType::ComputeShader, 1);
    }
}