#include "deferred-resolve.hpp"
#include "core/scene.hpp"

namespace engine::render
{
    direct3d::RenderTargetBase &DeferredResolve::Process(render::GBuffer &g_buffer, core::Scene *scene, ID3D11DepthStencilView *depth_target, ID3D11ShaderResourceView *depth_srv)
    {
        hdr_render_target_.SizeResources(g_buffer.albedo->size());

        const core::math::vec4 empty_vec(0, 0, 0, 0);

        direct3d::api().devcon4->OMSetRenderTargets(0, nullptr, nullptr);
        direct3d::api().devcon4->ClearRenderTargetView(hdr_render_target_.render_target_view(), empty_vec.data.data());
        direct3d::api().devcon4->RSSetState(direct3d::states().cull_none.ptr());

        std::vector<PointLightInstance> point_lights;
        std::vector<SpotLightInstance> spot_lights;

        {
            // TODO:
            // Move these settings to render pipeline
            // And add more easy way to access them
            // For now the exposure isn't dynamic, so the value is hardcoded
            // and the volume spheres may not be calculated correctly.
            const float exposure = 1.0f;
            auto &lrs = scene->renderer->light_render_system();
            auto &registry = scene->registry;
            constexpr auto inv_exposure = [] (float intensity, float exposure) -> float
            {
                return intensity * (78.0f / (0.65f * 100.0f)) * std::pow(2.0f, exposure);
            };

            constexpr auto calculate_length_for_solid_angle = [] (float radius, float x) -> float
            {
                return 2.0f * float(std::numbers::pi) * radius / std::sqrtf(4.0f * float(std::numbers::pi) * x - x * x);
            };

            for (auto const &entity : lrs.point_light_entities())
            {
                auto &light = registry.get<components::PointLight>(entity);
                auto &transform = registry.get<components::TransformComponent>(entity);
                auto instance_matrix = core::math::mat4::identity();
                instance_matrix = core::math::translate(instance_matrix, transform.position);
                float radius = length(transform.scale) / sqrtf(3.1f);
                float max_intensity = std::max(std::max(light.color.x, light.color.y), light.color.z) * light.power;

                float instance_scale = calculate_length_for_solid_angle(radius, 0.5f) / 10 * inv_exposure(max_intensity, exposure);
                instance_scale = std::clamp(instance_scale, 0.01f, 1e4f);
                instance_matrix = core::math::scale(instance_matrix, core::math::vec3(instance_scale));
                point_lights.emplace_back(
                    PointLightInstance
                    {
                        .transform_matrix = instance_matrix,
                        .color = light.color * light.power,
                        .radius = radius,
                        .position = transform.position,
                        .shadow_map_index = lrs.get_point_light_index(entity)
                    }
                );
            }
            for (auto const &entity : lrs.spot_light_entities())
            {
                auto &light = registry.get<components::SpotLight>(entity);
                auto &transform = registry.get<components::TransformComponent>(entity);

                auto instance_matrix = core::math::mat4::identity();
                instance_matrix = core::math::translate(instance_matrix, transform.position);
                float radius = length(transform.scale) / sqrtf(3.1f);
                float max_intensity = std::max(std::max(light.color.x, light.color.y), light.color.z) * light.power;

                float solid_angle = calculate_length_for_solid_angle(radius, 0.01f);
                float instance_scale = inv_exposure(max_intensity, exposure) * solid_angle / max_intensity;

                instance_matrix = core::math::scale(instance_matrix, core::math::vec3(instance_scale));

                spot_lights.emplace_back(
                    SpotLightInstance
                    {
                        .transform_matrix = instance_matrix,
                        .color = light.color * light.power,
                        .radius = radius,
                        .direction = transform.rotation * core::math::vec3(0, 1, 0),
                        .inner_cutoff = light.inner_cutoff,
                        .position = transform.position,
                        .outer_cutoff = light.outer_cutoff,
                        .shadow_map_index = lrs.get_spot_light_index(entity),
                    }
                );
            }
        }
        if (point_lights.size() > 0)
        {
            point_light_instance_buffer_.Init(std::span<PointLightInstance>(point_lights));
        }
        if (spot_lights.size() > 0)
        {
            spot_light_instance_buffer_.Init(std::span<SpotLightInstance>(spot_lights));
        }
        DeferredPerFrame per_frame;
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        prefiltered_texture_->GetDesc(&desc);
        per_frame.prefiltered_map_mip_levels = desc.TextureCube.MipLevels;
        auto &lrs = scene->renderer->light_render_system();

        per_frame_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        per_frame_buffer_.Update(per_frame);

        direct3d::api().devcon4->PSSetShaderResources(0, 1, &g_buffer.albedo->shader_resource_view());
        direct3d::api().devcon4->PSSetShaderResources(1, 1, &g_buffer.normals->shader_resource_view());
        direct3d::api().devcon4->PSSetShaderResources(2, 1, &g_buffer.roughness_metalness_transmittance_ao->shader_resource_view());
        direct3d::api().devcon4->PSSetShaderResources(3, 1, &g_buffer.emission->shader_resource_view());
        direct3d::api().devcon4->PSSetShaderResources(4, 1, &depth_srv);
        direct3d::api().devcon4->PSSetShaderResources(5, 1, &irradiance_texture_);
        direct3d::api().devcon4->PSSetShaderResources(6, 1, &prefiltered_texture_);
        direct3d::api().devcon4->PSSetShaderResources(7, 1, &brdf_texture_);
        lrs.BindPointShadowMaps(8);
        lrs.BindSpotShadowMaps(9);
        lrs.BindDirectionalShadowMaps(10);

        direct3d::api().devcon4->OMSetRenderTargets(1, &hdr_render_target_.render_target_view(), depth_target);

        direct3d::api().devcon4->OMSetBlendState(direct3d::states().additive_blend_state_no_alpha.ptr(), nullptr, 0xffffffff);

        environment_shader_.Bind();

        direct3d::api().devcon4->Draw(3, 0);

        environment_shader_.Unbind();

        auto &sphere = ModelSystem::GetModel(ModelSystem::GetUnitSphereLowPoly());
        if (point_lights.size() > 0 || spot_lights.size() > 0)
        {
            direct3d::api().devcon4->RSSetState(direct3d::states().cull_front.ptr());
        }
        if (point_lights.size() > 0)
        {
            volume_point_shader_.Bind();
            sphere.vertices.Bind(0);
            sphere.indices.Bind();
            point_light_instance_buffer_.Bind(1);
            for (auto &mesh : sphere.meshes)
            {
                direct3d::api().devcon4->DrawIndexedInstanced(mesh.mesh_range.index_count,
                                                              point_light_instance_buffer_.size(),
                                                              mesh.mesh_range.index_offset,
                                                              mesh.mesh_range.vertex_offset, 0);
            }
            volume_point_shader_.Unbind();
        }

        if (spot_lights.size() > 0)
        {
            volume_spot_shader_.Bind();
            sphere.vertices.Bind(0);
            sphere.indices.Bind();
            spot_light_instance_buffer_.Bind(1);
            for (auto &mesh : sphere.meshes)
            {
                direct3d::api().devcon4->DrawIndexedInstanced(mesh.mesh_range.index_count,
                                                              point_light_instance_buffer_.size(),
                                                              mesh.mesh_range.index_offset,
                                                              mesh.mesh_range.vertex_offset, 0);
            }
            volume_spot_shader_.Unbind();
        }

        direct3d::api().devcon4->PSSetShaderResources(0, 1, &direct3d::null_srv);
        direct3d::api().devcon4->PSSetShaderResources(1, 1, &direct3d::null_srv);

        direct3d::api().devcon4->PSSetShaderResources(2, 1, &direct3d::null_srv);
        direct3d::api().devcon4->PSSetShaderResources(3, 1, &direct3d::null_srv);
        direct3d::api().devcon4->PSSetShaderResources(4, 1, &direct3d::null_srv);

        direct3d::api().devcon4->PSSetShaderResources(8, 1, &direct3d::null_srv);
        direct3d::api().devcon4->PSSetShaderResources(9, 1, &direct3d::null_srv);
        direct3d::api().devcon4->PSSetShaderResources(10, 1, &direct3d::null_srv);

        direct3d::api().devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff);
        direct3d::api().devcon4->RSSetState(direct3d::states().cull_back.ptr());

        return hdr_render_target_;
    }
}